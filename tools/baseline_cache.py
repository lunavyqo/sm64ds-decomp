#!/usr/bin/env python3
"""Shared cache for ``tubuild.py linkcheck --baseline``.

Every worktree of one clone used to compile the stock control itself. The
control substitutes nothing, so worktrees on the same inputs produce the same
artefact. This module stores that artefact once, under the clone's common git
directory, and later worktrees copy it back into ``build/tu/_baseline/link``.

The key is a content hash, not a branch name. It covers the tracked inputs the
control compiles and links (``config/arm9``, a projection of the TU manifest,
``include/``, ``src/``, every tracked ``tools/**/*.py`` except tests, the
whole mwccarm tree, dsd, the extracted ROM) and whether the control builds a
ROM. A feature branch that does not touch those inputs reuses the baseline of
the main commit it forked from. A dirty input is hashed from the worktree
bytes, so it cannot hit a clean entry. Unreadable input refuses the cache and
the control is computed locally.

The manifest projection drops ``verification`` (including ``rom.sha256``).
``linkcheck --baseline`` passes ``intact_tus_override={}``, so
``rombuild.intact_tu_policies`` — the only reader of those fields — does not
run. See ``project_manifest_entry``.

``src_tu/`` is not part of the key unless a delinks entry names it. Agents
keep shadow translation units there, and those files are not in the stock link.

The lock is an exclusive lock file, not an advisory flock: a crashed agent
must leave a file the next one can remove. A dead owner's pid is stolen after
a short grace. A young lock whose pid is still alive is never stolen, even if
its mtime is old. A lock older than ``TUBUILD_BASELINE_LOCK_STALE`` (default
three hours) is stolen even if that pid is alive, so a hung owner cannot block
the machine forever; a lock we cannot attribute (another host, or an unreadable
file) is stolen only once it is that old. Waiters block until the owner
publishes or ``TUBUILD_BASELINE_LOCK_TIMEOUT`` (default one hour), then
compute locally without publishing.

Nothing here runs the compiler. ``tubuild.py`` does, and only on a miss.
"""
import argparse
import hashlib
import json
import os
import pathlib
import shutil
import socket
import subprocess
import sys
import time
import uuid

SCHEMA = 2
PREFIX = "baseline cache:"

# Directory trees keyed by their git tree id when clean. ``config/tu_manifest.d``
# is not here: its key is a projection, because a TU linkcheck rewrites
# verification fields the control does not read.
TRACKED_TREES = (
    "config/arm9",
    "include",
    "src",
    "mods",
)

TRACKED_FILES = (
    "config/rombuild-versions.txt",
    "config/rombuild-exclude.txt",
    "config/layout-known-issues.txt",
)

# Manifest entry fields the baseline control reads. Anything else is omitted
# from the key on purpose; see project_manifest_entry.
_MANIFEST_KEEP = (
    "id", "module", "status", "production_mode", "source", "promoted_source",
)
_SECTION_KEEP = ("name", "module_section", "start", "end")
_POLICY_KEEP = (
    "symbol", "disposition", "reason", "canonical_module", "canonical_address",
)

# Gitignored. Hashed from the bytes the control actually reads, following a
# worktree junction or symlink so every worktree that shares one ROM agrees.
EXTRACTED_TREES = (
    "extracted/dsd",
    "extracted/arm9",
    "extracted/arm9_overlays",
)

REPORT_NAME = "linkcheck.json"
ELF_NAME = "final_link.o"
CONFIG_REL = pathlib.PurePosixPath("config/arm9")

DEFAULT_LOCK_TIMEOUT = 3600.0
DEFAULT_LOCK_STALE = 3 * 3600.0
DEFAULT_LOCK_GRACE = 15.0


def _env_float(name, default):
    raw = os.environ.get(name, "").strip()
    if not raw:
        return default
    try:
        return float(raw)
    except ValueError:
        return default


def _git(repo, args, timeout=120, stdin=None):
    try:
        proc = subprocess.run(
            ["git", "-C", str(repo), *args],
            input=stdin, capture_output=True, timeout=timeout,
        )
    except (OSError, subprocess.TimeoutExpired) as exc:
        return 1, b"", str(exc).encode("utf-8", "replace")
    return proc.returncode, proc.stdout, proc.stderr


def git_common_dir(repo):
    """Absolute common git dir. Worktrees of one clone share it."""
    repo = pathlib.Path(repo)
    code, out, _err = _git(repo, ["rev-parse", "--git-common-dir"])
    if code != 0:
        return (repo / ".git").resolve()
    path = pathlib.Path(out.decode("utf-8", "replace").strip())
    if not path.is_absolute():
        path = (repo / path).resolve()
    return path


def default_cache_root(repo, env=None):
    """Where every worktree of this clone looks for a stored control.

    ``TUBUILD_BASELINE_CACHE`` wins, then a single path in
    ``tools/baseline-cache.path`` (gitignored; a local override), then
    ``<git-common-dir>/tu-baseline-cache``.
    """
    repo = pathlib.Path(repo)
    environ = os.environ if env is None else env
    override = str(environ.get("TUBUILD_BASELINE_CACHE", "")).strip()
    if override:
        return pathlib.Path(override)
    cfg = repo / "tools" / "baseline-cache.path"
    if cfg.is_file():
        for raw in cfg.read_text(encoding="utf-8").splitlines():
            line = raw.strip()
            if line and not line.startswith("#"):
                candidate = pathlib.Path(line)
                if not candidate.is_absolute():
                    candidate = (repo / candidate).resolve()
                return candidate
    return git_common_dir(repo) / "tu-baseline-cache"


def _parse_porcelain_z(blob):
    """Paths from ``git status --porcelain=v1 -z``. None if the record is torn."""
    paths = []
    i = 0
    while i < len(blob):
        if blob[i:i + 1] == b"\0":
            i += 1
            continue
        if i + 3 > len(blob) or blob[i + 2:i + 3] != b" ":
            return None
        xy = blob[i:i + 2]
        i += 3
        end = blob.find(b"\0", i)
        if end < 0:
            return None
        first = blob[i:end].decode("utf-8", "surrogateescape").replace("\\", "/")
        i = end + 1
        renamed = xy[0:1] in (b"R", b"C") or xy[1:2] in (b"R", b"C")
        if renamed:
            end2 = blob.find(b"\0", i)
            if end2 < 0:
                return None
            second = blob[i:end2].decode("utf-8", "surrogateescape").replace("\\", "/")
            i = end2 + 1
            paths.extend((first, second))
        else:
            paths.append(first)
    return paths


def _covers(rel, dirty_paths):
    prefix = rel.replace("\\", "/").rstrip("/")
    for path in dirty_paths:
        if path == prefix or path.startswith(prefix + "/"):
            return True
    return False


def _read_bytes(path):
    try:
        return pathlib.Path(path).read_bytes()
    except OSError:
        return None


def hash_tree(root):
    """Content hash of a file or directory, or None if a byte cannot be read.

    The top path is resolved so a worktree junction and a symlink to the same
    ROM hash alike. A symlink directory *inside* the tree refuses the hash:
    following it would leave the key depending on an unstated outside path.
    """
    root = pathlib.Path(root)
    if root.is_symlink() and not root.exists():
        return None
    if not root.exists():
        return "absent"
    scan = root.resolve()
    if not scan.exists():
        return None
    if scan.is_file():
        raw = _read_bytes(scan)
        if raw is None:
            return None
        return "file:" + hashlib.sha256(raw).hexdigest()
    if not scan.is_dir():
        return "absent"
    digest = hashlib.sha256()
    for path in sorted((p for p in scan.rglob("*")),
                       key=lambda p: p.relative_to(scan).as_posix()):
        if path.is_symlink() and not path.is_file():
            return None
        if not path.is_file():
            continue
        rel = path.relative_to(scan).as_posix().encode("utf-8", "surrogateescape")
        raw = _read_bytes(path)
        if raw is None:
            return None
        digest.update(len(rel).to_bytes(4, "big"))
        digest.update(rel)
        digest.update(len(raw).to_bytes(8, "big"))
        digest.update(raw)
    return "tree:" + digest.hexdigest()


def _toolchain_token(repo):
    """Hash dsd and every file under ``tools/mwccarm``. ``absent`` if none exist.

    The compiler tree is more than the two executables and the headers. A
    changed runtime DLL or license file changes what the control links, so the
    whole directory is in the key.
    """
    repo = pathlib.Path(repo)
    rows = []
    dsd = repo / "tools" / "bin" / "dsd.exe"
    if dsd.exists():
        rows.append(("tools/bin/dsd.exe", dsd))
    root = repo / "tools" / "mwccarm"
    if root.exists():
        if root.is_symlink() and not root.exists():
            return None
        scan = root.resolve()
        if scan.is_dir():
            for path in scan.rglob("*"):
                if path.is_symlink() and not path.is_file():
                    return None
                if not path.is_file():
                    continue
                rel = "tools/mwccarm/" + path.relative_to(scan).as_posix()
                rows.append((rel, path))
    if not rows:
        return "absent"
    digest = hashlib.sha256()
    for rel, path in sorted(rows):
        raw = _read_bytes(path)
        if raw is None:
            return None
        label = rel.encode("utf-8", "surrogateescape")
        digest.update(len(label).to_bytes(4, "big"))
        digest.update(label)
        digest.update(len(raw).to_bytes(8, "big"))
        digest.update(raw)
    return digest.hexdigest()


def _is_tool_py(rel):
    """Tracked pipeline source. Tests are not part of the control."""
    rel = rel.replace("\\", "/")
    if not rel.startswith("tools/") or not rel.endswith(".py"):
        return False
    name = rel.rsplit("/", 1)[-1]
    return not name.startswith("test_")


def project_manifest_entry(entry):
    """The part of one manifest entry that can change the baseline control.

    ``linkcheck --baseline`` demotes promoted intact-object entries using
    ``production_mode``, ``status``, ``source`` / ``promoted_source``, ``id``,
    ``module``, and section claims (``manifest_section_claims``). It compiles
    with ``intact_tus_override={}``, so ``rombuild.intact_tu_policies`` is not
    called. That function is the only reader of ``verification.linkcheck``,
    including ``rom.sha256``, phase flags, symbol inventories, and ``tuRanges``.
    A TU linkcheck rewrites those fields on the candidate it checked. They do
    not change the control's objects, link, or ROM.

    ``compiler_only_policies`` does run. It reads ``compiler_only_output``
    (symbol, disposition, reason, canonical home), the licensed function
    symbols, and — for ``deadstrip-data`` — ``module`` plus each section's
    name and address range. ``status`` is kept because demotion selects
    ``promoted``. A linkcheck moves status among text-verified, link-verified,
    and data-verified; a promotion writes ``promoted``, and that does change
    the control.

    Timestamps, notes, function addresses, and the whole ``verification``
    object are omitted. Two entries that differ only there hash the same.
    """
    if not isinstance(entry, dict):
        return None
    out = {key: entry[key] for key in _MANIFEST_KEEP if key in entry}
    sections = []
    for row in entry.get("sections") or []:
        if isinstance(row, dict):
            sections.append({key: row.get(key) for key in _SECTION_KEEP})
    if sections:
        out["sections"] = sections
    symbols = sorted({
        row.get("symbol") for row in (entry.get("functions") or [])
        if isinstance(row, dict) and row.get("symbol")
    })
    if symbols:
        out["functions"] = symbols
    policies = []
    for row in entry.get("compiler_only_output") or []:
        if isinstance(row, dict):
            policies.append({key: row.get(key) for key in _POLICY_KEEP})
    if policies:
        out["compiler_only_output"] = policies
    return out


def _manifest_token(repo):
    """Projection of ``config/tu_manifest.d``, from the bytes the control loads.

    Always the worktree, including when git says the tree is clean: the git
    tree id changes every time a TU records ``verification.linkcheck.rom``,
    and that is the miss this projection exists to avoid. ``_meta.json`` is
    not loaded into the policy.
    """
    root = pathlib.Path(repo) / "config" / "tu_manifest.d"
    if not root.exists():
        return "absent"
    if root.is_symlink() and not root.exists():
        return None
    scan = root.resolve()
    if not scan.is_dir():
        return None
    digest = hashlib.sha256()
    try:
        paths = sorted(
            (path for path in scan.rglob("*.json") if path.name != "_meta.json"),
            key=lambda path: path.relative_to(scan).as_posix(),
        )
    except OSError:
        return None
    for path in paths:
        if path.is_symlink() and not path.is_file():
            return None
        if not path.is_file():
            continue
        try:
            entry = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, UnicodeError, json.JSONDecodeError):
            return None
        projected = project_manifest_entry(entry)
        if projected is None:
            return None
        rel = path.relative_to(scan).as_posix().encode("utf-8", "surrogateescape")
        raw = json.dumps(projected, sort_keys=True, separators=(",", ":")).encode("utf-8")
        digest.update(len(rel).to_bytes(4, "big"))
        digest.update(rel)
        digest.update(len(raw).to_bytes(8, "big"))
        digest.update(raw)
    return "manifest:" + digest.hexdigest()


def _enrolled_src_tu(repo):
    """``src_tu/`` paths a delinks entry names. Anything else there is not linked."""
    root = pathlib.Path(repo) / "config" / "arm9"
    if not root.is_dir():
        return []
    found = set()
    try:
        files = sorted(root.rglob("delinks.txt"))
    except OSError:
        return None
    for dl in files:
        try:
            text = dl.read_text(encoding="utf-8", errors="replace")
        except OSError:
            return None
        for line in text.splitlines():
            if not line or line[0].isspace():
                continue
            stripped = line.strip().replace("\\", "/")
            if stripped.endswith(":") and stripped[:-1].startswith("src_tu/"):
                found.add(stripped[:-1])
    return sorted(found)


def _status_paths():
    """Pathspecs for the one ``git status`` call. Children are reported too."""
    return list(TRACKED_TREES) + list(TRACKED_FILES) + [
        "tools", "config/tu_manifest.d",
    ]


def _status_dirty(repo, rels=None):
    """Set of porcelain paths under ``rels``, or None if status cannot be trusted."""
    code, out, _err = _git(repo, [
        "status", "--porcelain=v1", "-z",
        "--untracked-files=all", "--ignored=no",
        "--", *(rels if rels is not None else _status_paths()),
    ])
    if code != 0:
        return None
    return _parse_porcelain_z(out)


def _parse_ls_tree_z(blob):
    """``git ls-tree -r -z`` records as ``{path: (type, oid)}``. None if torn."""
    found = {}
    i = 0
    while i < len(blob):
        end = blob.find(b"\0", i)
        if end < 0:
            return None
        rec = blob[i:end]
        i = end + 1
        if not rec:
            continue
        tab = rec.find(b"\t")
        if tab < 0:
            return None
        meta, path = rec[:tab], rec[tab + 1:]
        bits = meta.split(b" ")
        if len(bits) < 3:
            return None
        typ = bits[1].decode("ascii", "replace")
        oid = bits[2].decode("ascii", "replace")
        rel = path.decode("utf-8", "surrogateescape").replace("\\", "/")
        found[rel] = (typ, oid)
    return found


def _batch_check(repo, specs):
    """Resolve many git specs in one ``cat-file --batch-check``.

    Missing specs map to None. A torn or failed invocation returns None for
    the whole call, which refuses the cache rather than guessing an oid.
    """
    if not specs:
        return {}
    code, out, _err = _git(
        repo, ["cat-file", "--batch-check"],
        stdin="".join(spec + "\n" for spec in specs).encode(),
    )
    if code != 0:
        return None
    lines = out.splitlines()
    if len(lines) != len(specs):
        return None
    resolved = {}
    for spec, line in zip(specs, lines):
        text = line.decode("utf-8", "replace")
        if text.endswith(" missing"):
            resolved[spec] = None
            continue
        bits = text.split(" ")
        if len(bits) < 2:
            return None
        resolved[spec] = (bits[1], bits[0])
    return resolved


def _tool_index(repo):
    """Tracked ``tools/**/*.py`` except tests, from one ``ls-tree``.

    None when the listing is torn. An empty dict means the tree has no such
    files (a fresh test repo), which is a real answer.
    """
    code, out, _err = _git(repo, ["ls-tree", "-r", "-z", "HEAD", "--", "tools"])
    if code != 0:
        return None
    parsed = _parse_ls_tree_z(out)
    if parsed is None:
        return None
    return {rel: oid for rel, (typ, oid) in parsed.items()
            if typ == "blob" and _is_tool_py(rel)}


def main_commit_state_from(repo, checked):
    """``main`` tip, an older ``main`` commit, something else, or ``unknown``.

    ``checked`` is the ``cat-file`` map. ``origin/main`` is preferred and local
    ``main`` is the fallback. A feature branch is ``not-main`` even when its
    inputs match main; the content key still hits. This label is for the log.
    The ancestor test is the only extra git call, and only off the tip.
    """
    head_row = checked.get("HEAD") if checked else None
    if not head_row or head_row[0] != "commit":
        return "unknown", None
    head = head_row[1]
    # One ref is enough. origin/main wins; local main is only the fallback when
    # the remote ref is absent. A feature branch stops after that single
    # ancestor test instead of probing both.
    for spec in ("origin/main", "main"):
        row = checked.get(spec)
        if not row or row[0] != "commit":
            continue
        if row[1] == head:
            return "main", head
        if _git(repo, ["merge-base", "--is-ancestor", head, spec])[0] == 0:
            return "main-history", head
        return "not-main", head
    return "not-main", head


def main_commit_state(repo):
    """Log label for ``repo``. Prefer :func:`inspect_inputs`, which batches this."""
    checked = _batch_check(repo, ["HEAD", "origin/main", "main"])
    if checked is None:
        return "unknown", None
    return main_commit_state_from(repo, checked)


def _tracked_token(repo, rel, dirty, oid):
    """Git object id when the path is clean, worktree bytes when it is not.

    ``oid`` comes from the batched lookup. None means the path is not in HEAD.
    """
    path = pathlib.Path(repo) / rel
    if dirty is None or _covers(rel, dirty):
        if not path.exists():
            return "deleted" if dirty else "absent"
        return hash_tree(path)
    if oid:
        return "git:" + oid
    if not path.exists():
        return "absent"
    return hash_tree(path)


def _walk_tool_py(repo):
    """Worktree ``tools/**/*.py`` except tests. None if a directory symlink is inside."""
    root = pathlib.Path(repo) / "tools"
    if not root.exists():
        return []
    if root.is_symlink() and not root.exists():
        return None
    scan = root.resolve()
    if not scan.is_dir():
        return []
    found = []
    try:
        paths = list(scan.rglob("*.py"))
    except OSError:
        return None
    for path in paths:
        if path.is_symlink() and not path.is_file():
            return None
        if not path.is_file():
            continue
        rel = "tools/" + path.relative_to(scan).as_posix()
        if _is_tool_py(rel):
            found.append(rel)
    return found


def _tool_py_token(repo, index, dirty):
    """One token for every pipeline ``tools/**/*.py``.

    Clean files use the blob id from ``ls-tree``. Dirty and untracked files
    use worktree bytes, so a new module a worktree has not committed cannot
    reuse a control that was built without it. When ``git status`` itself
    cannot be trusted, the worktree is walked so an untracked module is not
    invisible.
    """
    paths = set(index or ())
    if dirty is None:
        walked = _walk_tool_py(repo)
        if walked is None:
            return None
        paths.update(walked)
    elif dirty:
        for rel in dirty:
            if _is_tool_py(rel):
                paths.add(rel)
    rows = []
    for rel in sorted(paths):
        use_bytes = dirty is None or rel in dirty
        if use_bytes:
            path = pathlib.Path(repo) / rel
            if not path.exists():
                token = "deleted" if dirty else "absent"
            else:
                token = hash_tree(path)
        else:
            oid = (index or {}).get(rel)
            token = "git:" + oid if oid else "absent"
        if token is None:
            return None
        rows.append(rel + "=" + token)
    return "|".join(rows) if rows else "none"


class InputSnapshot:
    """Everything the key is made of, computed once per invocation."""

    def __init__(self, parts, dirty, main_state, head, reason=None):
        self.parts = parts
        self.dirty = dirty
        self.main_state = main_state
        self.head = head
        self.reason = reason

    @property
    def ok(self):
        return self.reason is None


def _head_specs():
    return ["HEAD", "origin/main", "main"] + [
        f"HEAD:{rel}" for rel in list(TRACKED_TREES) + list(TRACKED_FILES)
    ]


def inspect_inputs(repo):
    """Hash the control's inputs. ``reason`` is set when the key must not be used.

    Git is three calls: one ``ls-tree`` for ``tools/**/*.py``, one
    ``cat-file --batch-check`` for HEAD and the tracked trees and files, and
    one ``git status``. A feature branch adds one ancestor test. Per-path
    ``rev-parse`` is what made a cache hit take most of a minute on Windows.
    """
    repo = pathlib.Path(repo)
    checked = _batch_check(repo, _head_specs())
    if checked is None:
        return InputSnapshot((), True, "unknown", None, "unreadable git index")
    state, head = main_commit_state_from(repo, checked)
    dirty_paths = _status_dirty(repo)
    dirty = dirty_paths is None or bool(dirty_paths)
    parts = []
    for rel in list(TRACKED_TREES) + list(TRACKED_FILES):
        row = checked.get(f"HEAD:{rel}")
        oid = row[1] if row else None
        token = _tracked_token(repo, rel, dirty_paths, oid)
        if token is None:
            return InputSnapshot((), dirty, state, head, f"unreadable input {rel}")
        parts.append((rel, token))
    manifest = _manifest_token(repo)
    if manifest is None:
        return InputSnapshot((), dirty, state, head, "unreadable input config/tu_manifest.d")
    parts.append(("config/tu_manifest.d", manifest))
    tool_index = _tool_index(repo)
    if tool_index is None:
        return InputSnapshot((), dirty, state, head, "unreadable tools listing")
    tools_py = _tool_py_token(repo, tool_index, dirty_paths)
    if tools_py is None:
        return InputSnapshot((), dirty, state, head, "unreadable tools/*.py")
    parts.append(("tools-py", tools_py))
    enrolled = _enrolled_src_tu(repo)
    if enrolled is None:
        return InputSnapshot((), dirty, state, head, "unreadable delinks while listing src_tu")
    src_tu_tokens = []
    for rel in enrolled:
        token = hash_tree(repo / rel)
        if token is None:
            return InputSnapshot((), dirty, state, head, f"unreadable input {rel}")
        src_tu_tokens.append(rel + "=" + token)
    parts.append(("enrolled-src_tu", "|".join(src_tu_tokens) if src_tu_tokens else "none"))
    for rel in EXTRACTED_TREES:
        token = hash_tree(repo / rel)
        if token is None:
            return InputSnapshot((), dirty, state, head, f"unreadable input {rel}")
        parts.append((rel, token))
    stock = repo / "build" / "sm64ds.nds"
    token = hash_tree(stock) if stock.exists() else "absent"
    if token is None:
        return InputSnapshot((), dirty, state, head, "unreadable input build/sm64ds.nds")
    parts.append(("build/sm64ds.nds", token))
    tools = _toolchain_token(repo)
    if tools is None:
        return InputSnapshot((), dirty, state, head, "unreadable compiler or dsd bytes")
    parts.append(("toolchain", tools))
    return InputSnapshot(tuple(parts), dirty, state, head, None)


def content_key(snapshot, *, build_rom):
    """64-hex key. ``build_rom`` is part of it: a ``--no-rom`` control has no ROM digest."""
    if not snapshot.ok:
        return None
    digest = hashlib.sha256()
    digest.update(f"schema={SCHEMA}\0buildRom={1 if build_rom else 0}\0".encode())
    for label, token in snapshot.parts:
        digest.update(label.encode("utf-8", "surrogateescape"))
        digest.update(b"\0")
        digest.update(str(token).encode("utf-8", "surrogateescape"))
        digest.update(b"\0")
    return digest.hexdigest()


def _pid_alive_posix(pid):
    """True when ``pid`` exists. ``os.kill(pid, 0)`` is a probe only on POSIX."""
    try:
        os.kill(pid, 0)
    except ProcessLookupError:
        return False
    except PermissionError:
        return True
    except OSError:
        return False
    return True


def _windows_query(pid):
    """``(exit_code, 0)`` or ``(None, winerror)`` via ``OpenProcess``.

    Signal 0 is ``CTRL_C_EVENT`` on Windows, and ``os.kill(pid, 0)`` delivers
    it. Probing the current process that way kills the interpreter
    (``0xC000013A``). Query the process instead.
    """
    import ctypes
    from ctypes import wintypes
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    kernel.OpenProcess.argtypes = (wintypes.DWORD, wintypes.BOOL, wintypes.DWORD)
    kernel.OpenProcess.restype = wintypes.HANDLE
    kernel.GetExitCodeProcess.argtypes = (
        wintypes.HANDLE, ctypes.POINTER(wintypes.DWORD))
    kernel.GetExitCodeProcess.restype = wintypes.BOOL
    kernel.CloseHandle.argtypes = (wintypes.HANDLE,)
    kernel.CloseHandle.restype = wintypes.BOOL
    handle = kernel.OpenProcess(0x1000, False, pid)  # PROCESS_QUERY_LIMITED_INFORMATION
    if not handle:
        return None, ctypes.get_last_error()
    code = wintypes.DWORD()
    try:
        if not kernel.GetExitCodeProcess(handle, ctypes.byref(code)):
            return None, ctypes.get_last_error()
        return int(code.value), 0
    finally:
        kernel.CloseHandle(handle)


def _pid_alive_windows(pid, query=None):
    """Alive when the process is still running or we may not query it.

    ``ERROR_ACCESS_DENIED`` (5) means the pid exists and this user cannot
    inspect it. ``ERROR_INVALID_PARAMETER`` (87) means it does not.
    ``STILL_ACTIVE`` is 259.
    """
    code, err = (query or _windows_query)(pid)
    if code is None:
        return err == 5
    return code == 259


def _pid_alive(pid):
    if not isinstance(pid, int) or pid <= 0:
        return False
    if os.name == "nt":
        return _pid_alive_windows(pid)
    return _pid_alive_posix(pid)


def _hostname():
    try:
        return socket.gethostname()
    except OSError:
        return "unknown"


class BaselineStore:
    """One cache root. Methods are safe across processes; the lock is a file."""

    def __init__(self, root):
        self.root = pathlib.Path(root)

    def entry_dir(self, key):
        return self.root / "entries" / key

    def _lock_path(self, key):
        return self.root / "locks" / key

    def _read_meta(self, directory):
        path = pathlib.Path(directory) / "meta.json"
        try:
            meta = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, UnicodeError, json.JSONDecodeError, TypeError):
            return None
        return meta if isinstance(meta, dict) else None

    def ready(self, key):
        directory = self.entry_dir(key)
        meta = self._read_meta(directory)
        if not meta or meta.get("schema") != SCHEMA or meta.get("key") != key:
            return False
        if not isinstance(meta.get("exitCode"), int):
            return False
        return (directory / REPORT_NAME).is_file() and (directory / ELF_NAME).is_file() \
            and (directory / "config" / "arm9").is_dir()

    def exit_code(self, key):
        meta = self._read_meta(self.entry_dir(key)) or {}
        code = meta.get("exitCode")
        return code if isinstance(code, int) else None

    def copy_to(self, key, dest):
        """Copy the stored control to ``dest``. The destination is replaced only
        after the copy is complete, so a crash does not leave a torn control."""
        src = self.entry_dir(key)
        if not self.ready(key):
            return False
        dest = pathlib.Path(dest)
        dest.parent.mkdir(parents=True, exist_ok=True)
        staging = dest.parent / f".{dest.name}.incoming-{os.getpid()}-{uuid.uuid4().hex}"
        try:
            staging.mkdir()
            shutil.copy2(src / REPORT_NAME, staging / REPORT_NAME)
            shutil.copy2(src / ELF_NAME, staging / ELF_NAME)
            shutil.copytree(src / "config", staging / "config")
            _replace_dir(staging, dest)
        except OSError:
            shutil.rmtree(staging, ignore_errors=True)
            return False
        return True

    def publish(self, key, source, *, exit_code, force, info, build_rom):
        """Store ``source``. ``force`` replaces a different stored verdict.

        Returns ``created``, ``replaced``, ``kept``, or ``conflict``.
        """
        source = pathlib.Path(source)
        if not _artefacts_present(source):
            return "missing"
        staging_root = self.root / "staging"
        staging_root.mkdir(parents=True, exist_ok=True)
        staging = staging_root / f"{key}.{os.getpid()}.{uuid.uuid4().hex}"
        final = self.entry_dir(key)
        try:
            staging.mkdir()
            shutil.copy2(source / REPORT_NAME, staging / REPORT_NAME)
            shutil.copy2(source / ELF_NAME, staging / ELF_NAME)
            shutil.copytree(source / "config", staging / "config")
            meta = {
                "schema": SCHEMA,
                "key": key,
                "exitCode": int(exit_code),
                "buildRom": bool(build_rom),
                "createdUnix": int(time.time()),
                "head": info.head,
                "mainCommit": info.main_state,
                "inputsDirty": bool(info.dirty),
            }
            (staging / "meta.json").write_text(
                json.dumps(meta, indent=2) + "\n", encoding="utf-8", newline="\n")
            final.parent.mkdir(parents=True, exist_ok=True)
            if final.exists():
                diffs = compare_baseline_dirs(final, staging)
                if not diffs:
                    shutil.rmtree(staging, ignore_errors=True)
                    return "kept"
                if not force:
                    shutil.rmtree(staging, ignore_errors=True)
                    return "conflict"
                backup = final.parent / f"{key}.replacing-{os.getpid()}"
                os.replace(final, backup)
                try:
                    os.replace(staging, final)
                except OSError:
                    if not final.exists() and backup.exists():
                        os.replace(backup, final)
                    raise
                shutil.rmtree(backup, ignore_errors=True)
                return "replaced"
            os.replace(staging, final)
            return "created"
        except OSError:
            shutil.rmtree(staging, ignore_errors=True)
            if final.exists():
                return "conflict"
            raise

    def _read_lock(self, path):
        try:
            text = path.read_text(encoding="utf-8")
        except OSError:
            return None
        info = {}
        for line in text.splitlines():
            if "=" not in line:
                continue
            k, v = line.split("=", 1)
            info[k] = v
        try:
            info["pid"] = int(info.get("pid", "0"))
            info["created"] = float(info.get("created", "0"))
        except ValueError:
            return None
        info["host"] = info.get("host", "")
        info["token"] = info.get("token", "")
        return info

    def _lock_stale(self, info, now, stale_s, grace_s):
        if info is None:
            return True
        age = now - info["created"]
        if age >= stale_s:
            return True
        if info["host"] != _hostname():
            return False
        return (not _pid_alive(info["pid"])) and age >= grace_s

    def try_acquire(self, key, *, now=None, stale_s, grace_s):
        """Take the lock. ``None`` if a live owner holds it, else ``(token, recovered)``."""
        now = time.time() if now is None else now
        path = self._lock_path(key)
        path.parent.mkdir(parents=True, exist_ok=True)
        token = uuid.uuid4().hex
        payload = (
            f"pid={os.getpid()}\n"
            f"host={_hostname()}\n"
            f"created={now:.3f}\n"
            f"token={token}\n"
        ).encode("utf-8")
        try:
            fd = os.open(str(path), os.O_CREAT | os.O_EXCL | os.O_WRONLY)
        except FileExistsError:
            info = self._read_lock(path)
            try:
                age_anchor = path.stat().st_mtime
            except OSError:
                return None
            if info is None:
                info = {"pid": 0, "host": "", "created": age_anchor, "token": ""}
            if not self._lock_stale(info, now, stale_s, grace_s):
                return None
            try:
                path.unlink()
            except OSError:
                return None
            try:
                fd = os.open(str(path), os.O_CREAT | os.O_EXCL | os.O_WRONLY)
            except FileExistsError:
                return None
            recovered = True
        else:
            recovered = False
        try:
            os.write(fd, payload)
        finally:
            os.close(fd)
        return token, recovered

    def release(self, key, token):
        if not token:
            return
        path = self._lock_path(key)
        info = self._read_lock(path)
        if info is None or info.get("token") != token:
            return
        try:
            path.unlink()
        except OSError:
            pass

    def acquire(self, key, *, timeout, poll_s, stale_s, grace_s, log):
        """Block until this process holds the lock or ``timeout`` elapses.

        Returns the lock token, or ``None`` on timeout. The caller must
        ``release``. A stored entry is not special here: the caller re-checks
        ``ready`` after the lock is taken so two waiters cannot both build.
        """
        deadline = time.time() + timeout
        while True:
            got = self.try_acquire(key, stale_s=stale_s, grace_s=grace_s)
            if got is not None:
                token, recovered = got
                if recovered:
                    log(f"{PREFIX} stale lock recovered for key {key}")
                return token
            if time.time() >= deadline:
                return None
            time.sleep(poll_s)


def _replace_dir(src, dest):
    """Move directory ``src`` onto ``dest``. ``src`` is consumed."""
    dest = pathlib.Path(dest)
    src = pathlib.Path(src)
    if not dest.exists():
        os.replace(src, dest)
        return
    backup = dest.parent / f".{dest.name}.old-{os.getpid()}-{uuid.uuid4().hex}"
    os.replace(dest, backup)
    try:
        os.replace(src, dest)
    except OSError:
        if not dest.exists() and backup.exists():
            os.replace(backup, dest)
        raise
    shutil.rmtree(backup, ignore_errors=True)


def _artefacts_present(directory):
    directory = pathlib.Path(directory)
    return (directory / REPORT_NAME).is_file() and (directory / ELF_NAME).is_file() \
        and (directory / "config" / "arm9").is_dir()


def _load_report(directory):
    try:
        report = json.loads((pathlib.Path(directory) / REPORT_NAME).read_text(encoding="utf-8"))
    except (OSError, UnicodeError, json.JSONDecodeError, TypeError):
        return None
    return report if isinstance(report, dict) else None


def project_verdict(report):
    """The part of a control report two runs must share.

    Phase timings and compile-cache hit counts are not part of the verdict:
    a warm object cache and a cold one link the same bytes.
    """
    phases = {}
    for name, phase in (report.get("phases") or {}).items():
        if not isinstance(phase, dict):
            phases[name] = phase
            continue
        item = {"ok": phase.get("ok")}
        if name == "checkSymbols":
            item["errors"] = phase.get("errors")
        phases[name] = item
    analysis = report.get("analysis") or {}
    rom = report.get("rom") if isinstance(report.get("rom"), dict) else {}
    fidelity = analysis.get("moduleFidelity")
    return {
        "baseline": report.get("baseline"),
        "result": report.get("result"),
        "phases": phases,
        "analysis": {
            "passed": analysis.get("passed"),
            "moduleFidelity": fidelity,
            "sourceBuild": analysis.get("sourceBuild"),
            "missingModuleBinaries": analysis.get("missingModuleBinaries"),
            "failures": analysis.get("failures"),
        },
        "rom": {k: rom.get(k) for k in ("bytes", "sha256", "matchesStockRom")},
        "intactTusDemoted": report.get("intactTusDemoted"),
        "baselineEvidence": report.get("baselineEvidence"),
        "symbolsNew": report.get("symbolsNew"),
    }


def _diff_obj(prefix, left, right, out, limit):
    if len(out) >= limit:
        return
    if left == right:
        return
    if isinstance(left, dict) and isinstance(right, dict):
        for key in sorted(set(left) | set(right)):
            child = f"{prefix}.{key}" if prefix else str(key)
            _diff_obj(child, left.get(key), right.get(key), out, limit)
        return
    out.append(f"{prefix}: {left!r} != {right!r}")


def compare_baseline_dirs(left, right, *, limit=12):
    """Human-readable differences. Empty means the verdicts and artefacts match."""
    left, right = pathlib.Path(left), pathlib.Path(right)
    diffs = []
    for directory, label in ((left, "left"), (right, "right")):
        if not _artefacts_present(directory):
            diffs.append(f"{label} baseline is missing linkcheck.json, final_link.o, "
                         f"or config/arm9")
    if diffs:
        return diffs
    left_elf = hashlib.sha256((left / ELF_NAME).read_bytes()).hexdigest()
    right_elf = hashlib.sha256((right / ELF_NAME).read_bytes()).hexdigest()
    if left_elf != right_elf:
        diffs.append(f"final_link.o sha256: {left_elf} != {right_elf}")
    left_cfg = hash_tree(left / "config" / "arm9")
    right_cfg = hash_tree(right / "config" / "arm9")
    if left_cfg != right_cfg:
        diffs.append(f"config/arm9: {left_cfg} != {right_cfg}")
    left_report = _load_report(left)
    right_report = _load_report(right)
    if left_report is None or right_report is None:
        diffs.append("linkcheck.json is unreadable")
        return diffs
    _diff_obj("", project_verdict(left_report), project_verdict(right_report), diffs, limit)
    if len(diffs) >= limit:
        diffs.append(f"(difference list truncated at {limit})")
    return diffs


def control_summary(report):
    """The ``BASELINE CONTROL:`` line ``cmd_linkcheck`` prints, from a stored report."""
    analysis = report.get("analysis") or {}
    fidelity = analysis.get("moduleFidelity") or {}
    results = fidelity.get("results") or []
    bad = [row.get("module") for row in results
           if isinstance(row, dict) and not row.get("exact")]
    module_ok = bool(analysis.get("passed")) and not bad
    symbols_ok = ((report.get("phases") or {}).get("checkSymbols") or {}).get("ok")
    rom = (report.get("phases") or {}).get("rom")
    if rom is None:
        rom_word = "skipped"
    elif isinstance(rom, dict) and rom.get("ok"):
        rom_word = "built"
    else:
        rom_word = "not built"
    return (f"BASELINE CONTROL: modules {'PASS' if module_ok else 'FAIL'}, "
            f"dsd check symbols --fail {'PASS' if symbols_ok else 'FAIL'}, "
            f"ROM {rom_word}.")


def _context_phrase(info):
    head = info.head or "unknown"
    dirty = "dirty" if info.dirty else "clean"
    return f"HEAD {head}, {info.main_state}, inputs {dirty}"


class BaselineSession:
    """One ``linkcheck --baseline`` invocation's relationship to the cache.

    ``cached_exit`` is set when the caller should return that code and not
    build. ``finish(exit_code)`` publishes a build and always drops the lock.
    ``override_exit`` replaces the build's code after ``--verify-baseline-cache``.
    """

    def __init__(self, store, dest, key, info, build_rom, log):
        self.store = store
        self.dest = pathlib.Path(dest)
        self.key = key
        self.info = info
        self.build_rom = build_rom
        self.log = log
        self.cached_exit = None
        self.override_exit = None
        self._token = None
        self._mode = "build"
        self._previous = None
        self.rebuild = False

    def finish(self, exit_code):
        try:
            self._finish(exit_code)
        finally:
            if self.store is not None and self.key and self._token:
                self.store.release(self.key, self._token)
                self._token = None
            if self._previous is not None:
                shutil.rmtree(self._previous, ignore_errors=True)
                self._previous = None

    def _finish(self, exit_code):
        if self._mode in ("disabled", "hit", "unkeyed", "timeout"):
            return
        if self._mode == "verify":
            self._finish_verify(exit_code)
            return
        if exit_code != 0 or not _artefacts_present(self.dest):
            return
        result = self.store.publish(
            self.key, self.dest, exit_code=exit_code, force=self.rebuild,
            info=self.info, build_rom=self.build_rom)
        where = self.store.entry_dir(self.key)
        if result == "conflict":
            self.log(f"{PREFIX} kept the stored entry; this build differs for key "
                     f"{self.key} ({_context_phrase(self.info)})")
            return
        if result in ("created", "replaced", "kept"):
            self.log(f"{PREFIX} computed key {self.key} and stored {where} "
                     f"({_context_phrase(self.info)})")

    def _finish_verify(self, exit_code):
        if self._previous is None:
            if exit_code == 0 and _artefacts_present(self.dest):
                self.store.publish(
                    self.key, self.dest, exit_code=exit_code, force=True,
                    info=self.info, build_rom=self.build_rom)
                self.log(f"{PREFIX} verify stored a new baseline for key {self.key}; "
                         f"there was no previous entry to compare")
            self.override_exit = 2
            return
        if not _artefacts_present(self.dest):
            self.log(f"{PREFIX} verify could not compare key {self.key}; "
                     f"the fresh build produced no control")
            self.override_exit = 1 if exit_code == 0 else exit_code
            return
        diffs = compare_baseline_dirs(self._previous, self.dest)
        if diffs:
            self.log(f"{PREFIX} verify differs key {self.key}")
            for line in diffs:
                self.log(f"{PREFIX}   {line}")
            self.override_exit = 1
            return
        self.store.publish(
            self.key, self.dest, exit_code=0, force=False,
            info=self.info, build_rom=self.build_rom)
        self.log(f"{PREFIX} verify matches key {self.key}")
        if exit_code != 0:
            self.override_exit = exit_code


def open_baseline_session(repo, dest, *, enabled=True, rebuild=False, verify=False,
                          build_rom=True, cache_root=None, lock_timeout=None,
                          lock_stale=None, lock_grace=DEFAULT_LOCK_GRACE,
                          poll_s=0.25, validate=None, log=print):
    """Decide whether this baseline is already stored. See :class:`BaselineSession`."""
    repo = pathlib.Path(repo)
    dest = pathlib.Path(dest)
    root = pathlib.Path(cache_root) if cache_root is not None else default_cache_root(repo)
    session = BaselineSession(None, dest, None, None, build_rom, log)
    if not enabled:
        session._mode = "disabled"
        log(f"{PREFIX} disabled (--no-baseline-cache)")
        return session
    info = inspect_inputs(repo)
    if not info.ok:
        session._mode = "unkeyed"
        session.info = info
        log(f"{PREFIX} not used ({info.reason}); computing")
        return session
    key = content_key(info, build_rom=build_rom)
    store = BaselineStore(root)
    session.store = store
    session.key = key
    session.info = info
    session.rebuild = bool(rebuild) and not verify
    timeout = DEFAULT_LOCK_TIMEOUT if lock_timeout is None else lock_timeout
    stale = DEFAULT_LOCK_STALE if lock_stale is None else lock_stale

    def consider_hit():
        if not store.ready(key):
            return False
        if not store.copy_to(key, dest):
            return False
        if validate is not None and not validate(dest):
            shutil.rmtree(dest, ignore_errors=True)
            log(f"{PREFIX} stored entry failed content check; recomputing key {key}")
            return False
        report = _load_report(dest) or {}
        log(f"{PREFIX} reused key {key} from {store.entry_dir(key)} "
            f"({_context_phrase(info)})")
        log(control_summary(report))
        log("This is the same scratch pipeline with NO TU substitution: anything failing "
            "here is pre-existing and belongs to the tree, not to any TU.")
        session.cached_exit = store.exit_code(key)
        session._mode = "hit"
        return True

    if not rebuild and not verify and consider_hit():
        return session
    if verify:
        if store.ready(key):
            previous = root / "staging" / f"verify-{key}-{os.getpid()}-{uuid.uuid4().hex}"
            if store.copy_to(key, previous):
                session._previous = previous
        log(f"{PREFIX} verifying key {key} against a fresh build")
        session._mode = "verify"
    elif rebuild:
        log(f"{PREFIX} rebuilding key {key} (--rebuild-baseline)")
    token = store.acquire(key, timeout=timeout, poll_s=poll_s, stale_s=stale,
                          grace_s=lock_grace, log=log)
    if token is None:
        session._mode = "timeout"
        log(f"{PREFIX} lock timeout after {timeout:g}s for key {key}; "
            f"computing locally without publishing")
        return session
    session._token = token
    if not rebuild and not verify and consider_hit():
        store.release(key, token)
        session._token = None
        return session
    if not verify and not rebuild:
        log(f"{PREFIX} computing key {key} ({_context_phrase(info)})")
    return session


def restore_cached_baseline(repo, dest, *, enabled=True, cache_root=None,
                            validate=None, log=print):
    """Copy a stored control into ``dest`` for a TU linkcheck that did not build one.

    A current local control is left alone. A full-ROM control is preferred over
    a ``--no-rom`` one. Returns the key that was restored, or None.
    """
    if not enabled:
        return None
    dest = pathlib.Path(dest)
    if _artefacts_present(dest) and (validate is None or validate(dest)):
        return None
    repo = pathlib.Path(repo)
    info = inspect_inputs(repo)
    if not info.ok:
        log(f"{PREFIX} not used for TU restore ({info.reason})")
        return None
    root = pathlib.Path(cache_root) if cache_root is not None else default_cache_root(repo)
    store = BaselineStore(root)
    for build_rom in (True, False):
        key = content_key(info, build_rom=build_rom)
        if not store.ready(key):
            continue
        if not store.copy_to(key, dest):
            continue
        if validate is not None and not validate(dest):
            shutil.rmtree(dest, ignore_errors=True)
            log(f"{PREFIX} stored entry failed content check; left the local control "
                f"unset for key {key}")
            continue
        log(f"{PREFIX} restored local control key {key} from {store.entry_dir(key)} "
            f"({_context_phrase(info)})")
        return key
    return None


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    sub = ap.add_subparsers(dest="cmd", required=True)
    key = sub.add_parser("key", help="print the baseline content key for this worktree")
    key.add_argument("--no-rom", action="store_true")
    key.add_argument("--repo", type=pathlib.Path, default=None)
    chk = sub.add_parser("check", help="compare two stored or scratch baseline directories")
    chk.add_argument("left", type=pathlib.Path)
    chk.add_argument("right", type=pathlib.Path)
    args = ap.parse_args(argv)
    if args.cmd == "key":
        repo = args.repo or pathlib.Path(__file__).resolve().parent.parent
        info = inspect_inputs(repo)
        if not info.ok:
            print(f"{PREFIX} no key ({info.reason})", file=sys.stderr)
            return 2
        print(content_key(info, build_rom=not args.no_rom))
        print(f"{_context_phrase(info)}", file=sys.stderr)
        return 0
    diffs = compare_baseline_dirs(args.left, args.right)
    if diffs:
        print(f"{PREFIX} differ")
        for line in diffs:
            print(f"{PREFIX}   {line}")
        return 1
    print(f"{PREFIX} match")
    return 0


if __name__ == "__main__":
    sys.exit(main())
