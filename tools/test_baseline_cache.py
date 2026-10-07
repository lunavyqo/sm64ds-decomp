"""The shared linkcheck baseline is keyed on inputs, and a false hit is a miss.

A worktree that reused a control built from different source, config, or
compiler bytes would grade its TU against the wrong tree and still look green.
These tests mock the compile. They check the key, the lock, and the reuse.
"""
import io
import os
import pathlib
import shutil
import socket
import subprocess
import sys
import tempfile
import threading
import time
import unittest
from contextlib import redirect_stderr, redirect_stdout

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))
import baseline_cache as BC  # noqa: E402


def _git(repo, *args):
    subprocess.run(["git", "-C", str(repo), *args], check=True, capture_output=True)


def _init_repo(path):
    path.mkdir(parents=True, exist_ok=True)
    _git(path, "init", "-q")
    _git(path, "config", "user.email", "baseline-cache@example.com")
    _git(path, "config", "user.name", "baseline cache tests")
    _git(path, "config", "commit.gpgsign", "false")
    for rel in ("src", "include", "config/arm9", "config/tu_manifest.d"):
        (path / rel).mkdir(parents=True)
    (path / "src" / "a.c").write_text("int a;\n", encoding="utf-8", newline="\n")
    (path / "include" / "a.h").write_text("#pragma once\n", encoding="utf-8", newline="\n")
    (path / "config" / "arm9" / "delinks.txt").write_text(
        "src/a.c:\n", encoding="utf-8", newline="\n")
    (path / "config" / "rombuild-versions.txt").write_text("v\n", encoding="utf-8", newline="\n")
    (path / "config" / "rombuild-exclude.txt").write_text("\n", encoding="utf-8", newline="\n")
    _git(path, "add", "-A")
    _git(path, "commit", "-q", "-m", "base")


def _key(repo, build_rom=True):
    info = BC.inspect_inputs(repo)
    if not info.ok:
        raise AssertionError(info.reason)
    return BC.content_key(info, build_rom=build_rom), info


def _report(seconds=1.0, errors=None, module="ov002"):
    return {
        "baseline": True,
        "result": "failed",
        "module": module,
        "phases": {
            "link": {"ok": True, "seconds": seconds, "cacheHits": 3},
            "checkSymbols": {"ok": False, "seconds": seconds,
                             "errors": ["pre-existing"] if errors is None else errors},
        },
        "analysis": {
            "passed": True,
            "moduleFidelity": {"results": [{"module": "ov002", "exact": True}]},
            "sourceBuild": True,
            "missingModuleBinaries": [],
            "failures": [],
        },
        "rom": {"bytes": 8, "sha256": "ab", "matchesStockRom": False},
        "intactTusDemoted": [],
        "baselineEvidence": {"linkedElfSha256": "stored"},
        "symbolsNew": None,
    }


def _write_control(dest, elf=b"ELF-A", config_text="sym\n", report=None):
    dest = pathlib.Path(dest)
    if dest.exists():
        shutil.rmtree(dest)
    (dest / "config" / "arm9").mkdir(parents=True)
    (dest / "config" / "arm9" / "symbols.txt").write_text(
        config_text, encoding="utf-8", newline="\n")
    (dest / "final_link.o").write_bytes(elf)
    import json
    (dest / "linkcheck.json").write_text(
        json.dumps(report or _report()) + "\n", encoding="utf-8", newline="\n")


class KeyTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name) / "repo"
        _init_repo(self.repo)

    def test_the_same_tree_keeps_one_key(self):
        first, info = _key(self.repo)
        second, _info = _key(self.repo)
        self.assertEqual(first, second)
        self.assertEqual(len(first), 64)
        self.assertFalse(info.dirty)
        self.assertIn(info.main_state, ("main", "not-main", "main-history", "unknown"))

    def test_two_repos_with_the_same_inputs_share_a_key(self):
        other = pathlib.Path(self.tmp.name) / "other"
        _init_repo(other)
        self.assertEqual(_key(self.repo)[0], _key(other)[0])

    def test_a_source_edit_changes_the_key_and_a_dirty_tree_is_stable(self):
        clean = _key(self.repo)[0]
        path = self.repo / "src" / "a.c"
        path.write_text("int a;\nint b;\n", encoding="utf-8", newline="\n")
        dirty, info = _key(self.repo)
        self.assertNotEqual(clean, dirty)
        self.assertTrue(info.dirty)
        self.assertEqual(dirty, _key(self.repo)[0])
        _git(self.repo, "add", "-A")
        _git(self.repo, "commit", "-q", "-m", "edit")
        committed, info = _key(self.repo)
        self.assertNotEqual(committed, dirty)
        self.assertNotEqual(committed, clean)
        self.assertFalse(info.dirty)

    def test_crlf_bytes_do_not_reuse_the_lf_commit(self):
        clean = _key(self.repo)[0]
        (self.repo / "src" / "a.c").write_bytes(b"int a;\r\nint b;\r\n")
        self.assertNotEqual(_key(self.repo)[0], clean)

    def test_an_unenrolled_src_tu_shadow_does_not_change_the_key(self):
        before = _key(self.repo)[0]
        shadow = self.repo / "src_tu" / "ov002" / "Shadow.cpp"
        shadow.parent.mkdir(parents=True)
        shadow.write_text("void shadow() {}\n", encoding="utf-8", newline="\n")
        self.assertEqual(_key(self.repo)[0], before)

    def test_an_enrolled_src_tu_path_changes_the_key(self):
        before = _key(self.repo)[0]
        dl = self.repo / "config" / "arm9" / "delinks.txt"
        dl.write_text("src/a.c:\nsrc_tu/ov002/Shadow.cpp:\n",
                      encoding="utf-8", newline="\n")
        named = _key(self.repo)[0]
        self.assertNotEqual(named, before)
        shadow = self.repo / "src_tu" / "ov002" / "Shadow.cpp"
        shadow.parent.mkdir(parents=True)
        shadow.write_text("void shadow() {}\n", encoding="utf-8", newline="\n")
        self.assertNotEqual(_key(self.repo)[0], named)

    def test_toolchain_bytes_and_build_rom_are_in_the_key(self):
        base = _key(self.repo)[0]
        self.assertNotEqual(_key(self.repo, build_rom=False)[0], base)
        dsd = self.repo / "tools" / "bin" / "dsd.exe"
        dsd.parent.mkdir(parents=True)
        dsd.write_bytes(b"dsd-v1")
        with_dsd = _key(self.repo)[0]
        self.assertNotEqual(with_dsd, base)
        dsd.write_bytes(b"dsd-v2")
        with_dsd2 = _key(self.repo)[0]
        self.assertNotEqual(with_dsd2, with_dsd)
        compiler = self.repo / "tools" / "mwccarm" / "2004" / "b56" / "mwccarm.exe"
        compiler.parent.mkdir(parents=True)
        compiler.write_bytes(b"mwcc")
        self.assertNotEqual(_key(self.repo)[0], with_dsd2)

    def test_extracted_bytes_and_the_stock_rom_change_the_key(self):
        before = _key(self.repo)[0]
        extracted = self.repo / "extracted" / "dsd" / "config.yaml"
        extracted.parent.mkdir(parents=True)
        extracted.write_text("files: []\n", encoding="utf-8", newline="\n")
        with_rom = _key(self.repo)[0]
        self.assertNotEqual(with_rom, before)
        extracted.write_text("files: [a]\n", encoding="utf-8", newline="\n")
        changed = _key(self.repo)[0]
        self.assertNotEqual(changed, with_rom)
        overlay = self.repo / "extracted" / "arm9_overlays" / "ov002.bin"
        overlay.parent.mkdir(parents=True)
        overlay.write_bytes(b"ov")
        self.assertNotEqual(_key(self.repo)[0], changed)
        stock = self.repo / "build" / "sm64ds.nds"
        stock.parent.mkdir(parents=True)
        stock.write_bytes(b"nds")
        with_stock = _key(self.repo)[0]
        stock.write_bytes(b"nds2")
        self.assertNotEqual(_key(self.repo)[0], with_stock)

    def test_a_nested_directory_symlink_refuses_the_key(self):
        extracted = self.repo / "extracted" / "dsd"
        extracted.mkdir(parents=True)
        try:
            (extracted / "outside").symlink_to(self.repo / "src", target_is_directory=True)
        except OSError:
            self.skipTest("symlinks are not available")
        info = BC.inspect_inputs(self.repo)
        self.assertFalse(info.ok)
        self.assertIn("unreadable", info.reason)

    def test_a_top_level_symlink_is_followed(self):
        real = pathlib.Path(self.tmp.name) / "real-extracted"
        (real / "dsd").mkdir(parents=True)
        (real / "dsd" / "config.yaml").write_text("a: 1\n", encoding="utf-8", newline="\n")
        link = self.repo / "extracted"
        try:
            link.symlink_to(real, target_is_directory=True)
        except OSError:
            self.skipTest("symlinks are not available")
        self.assertEqual(BC.hash_tree(link / "dsd"), BC.hash_tree(real / "dsd"))
        info = BC.inspect_inputs(self.repo)
        self.assertTrue(info.ok, info.reason)

    def test_tool_list_covers_the_fingerprint_tools(self):
        import tubuild
        names = {path.name for path in tubuild.BASELINE_CONTROL_TOOLS}
        covered = {pathlib.PurePosixPath(rel).name for rel in BC.TOOL_FILES}
        self.assertTrue(names <= covered)


class CacheRootTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name) / "repo"
        _init_repo(self.repo)

    def test_worktrees_share_the_common_git_dir(self):
        other = pathlib.Path(self.tmp.name) / "worktree"
        _git(self.repo, "worktree", "add", "--detach", "-q", str(other), "HEAD")
        self.assertEqual(BC.git_common_dir(self.repo), BC.git_common_dir(other))
        self.assertEqual(
            BC.default_cache_root(self.repo, env={}),
            BC.default_cache_root(other, env={}))
        self.assertEqual(
            BC.default_cache_root(self.repo, env={}).name, "tu-baseline-cache")

    def test_env_and_path_file_override_the_common_dir(self):
        chosen = pathlib.Path(self.tmp.name) / "custom cache"
        self.assertEqual(
            BC.default_cache_root(self.repo, env={"TUBUILD_BASELINE_CACHE": str(chosen)}),
            chosen)
        cfg = self.repo / "tools" / "baseline-cache.path"
        cfg.parent.mkdir(parents=True, exist_ok=True)
        cfg.write_text("# comment\n../shared-cache\n", encoding="utf-8", newline="\n")
        resolved = (self.repo / "../shared-cache").resolve()
        self.assertEqual(BC.default_cache_root(self.repo, env={}), resolved)


class StoreTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = pathlib.Path(self.tmp.name) / "cache"
        self.store = BC.BaselineStore(self.root)
        self.left = pathlib.Path(self.tmp.name) / "left"
        self.right = pathlib.Path(self.tmp.name) / "right"
        self.info = BC.InputSnapshot((), False, "main", "abc", None)

    def test_verdict_ignores_timings_and_the_module_label(self):
        _write_control(self.left, report=_report(seconds=1.5, module="ov002"))
        _write_control(self.right, report=_report(seconds=400.0, module="ov099"))
        self.assertEqual(BC.compare_baseline_dirs(self.left, self.right), [])
        _write_control(self.right, report=_report(errors=["different"]))
        self.assertTrue(BC.compare_baseline_dirs(self.left, self.right))

    def test_publish_keeps_a_different_verdict_unless_forced(self):
        _write_control(self.left, elf=b"ELF-A")
        _write_control(self.right, elf=b"ELF-B")
        self.assertEqual(
            self.store.publish("k" * 64, self.left, exit_code=0, force=False,
                               info=self.info, build_rom=True),
            "created")
        self.assertEqual(
            self.store.publish("k" * 64, self.right, exit_code=0, force=False,
                               info=self.info, build_rom=True),
            "conflict")
        self.assertEqual((self.store.entry_dir("k" * 64) / "final_link.o").read_bytes(), b"ELF-A")
        self.assertEqual(
            self.store.publish("k" * 64, self.right, exit_code=0, force=True,
                               info=self.info, build_rom=True),
            "replaced")
        self.assertEqual((self.store.entry_dir("k" * 64) / "final_link.o").read_bytes(), b"ELF-B")

    def test_an_entry_without_meta_is_not_ready(self):
        key = "m" * 64
        _write_control(self.store.entry_dir(key))
        self.assertFalse(self.store.ready(key))

    def test_a_dead_pid_is_recovered_and_a_live_one_is_not(self):
        proc = subprocess.run(
            [sys.executable, "-c", "import os; print(os.getpid())"],
            check=True, capture_output=True, text=True)
        dead = int(proc.stdout.strip())
        key = "d" * 64
        path = self.store._lock_path(key)
        path.parent.mkdir(parents=True)
        path.write_text(
            f"pid={dead}\nhost={socket.gethostname()}\n"
            f"created={time.time() - 60:.3f}\ntoken=old\n",
            encoding="utf-8", newline="\n")
        os.utime(path, (time.time() - 60, time.time() - 60))
        logs = []
        token = self.store.acquire(
            key, timeout=1, poll_s=0.01, stale_s=10_000, grace_s=15, log=logs.append)
        self.assertIsNotNone(token)
        self.assertTrue(any("stale lock recovered" in line for line in logs))
        self.store.release(key, token)

        live = "e" * 64
        live_path = self.store._lock_path(live)
        live_path.write_text(
            f"pid={os.getpid()}\nhost={socket.gethostname()}\n"
            f"created={time.time():.3f}\ntoken=held\n",
            encoding="utf-8", newline="\n")
        os.utime(live_path, (time.time() - 3600, time.time() - 3600))
        self.assertIsNone(self.store.try_acquire(live, stale_s=10_000, grace_s=15))

    def test_a_hung_live_pid_is_recovered_after_the_stale_age(self):
        key = "h" * 64
        path = self.store._lock_path(key)
        path.parent.mkdir(parents=True)
        path.write_text(
            f"pid={os.getpid()}\nhost={socket.gethostname()}\n"
            f"created={time.time() - 100:.3f}\ntoken=hung\n",
            encoding="utf-8", newline="\n")
        got = self.store.try_acquire(key, stale_s=10, grace_s=15)
        self.assertIsNotNone(got)

    def test_check_cli_reports_a_match(self):
        _write_control(self.left)
        _write_control(self.right)
        out = io.StringIO()
        with redirect_stdout(out):
            code = BC.main(["check", str(self.left), str(self.right)])
        self.assertEqual(code, 0)
        self.assertIn("match", out.getvalue())


class SessionTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.repo = pathlib.Path(self.tmp.name) / "repo"
        _init_repo(self.repo)
        self.cache = pathlib.Path(self.tmp.name) / "cache dir"
        self.dest = pathlib.Path(self.tmp.name) / "local"
        self.logs = []

    def _open(self, **kw):
        opts = dict(
            enabled=True, rebuild=False, verify=False, build_rom=True,
            cache_root=self.cache, lock_timeout=5, lock_stale=3600, lock_grace=15,
            poll_s=0.02, validate=lambda path: True, log=self.logs.append,
        )
        opts.update(kw)
        return BC.open_baseline_session(self.repo, self.dest, **opts)

    def _build(self, session, elf=b"ELF-A", code=0):
        _write_control(self.dest, elf=elf)
        session.finish(code)
        if session.override_exit is not None:
            return session.override_exit
        return code

    def test_the_second_caller_reuses_and_the_build_runs_once(self):
        first = self._open()
        self.assertIsNone(first.cached_exit)
        self.assertTrue(any(line.startswith("baseline cache: computing key ")
                            for line in self.logs))
        self._build(first)
        self.assertTrue(any(line.startswith("baseline cache: computed key ")
                            for line in self.logs))
        self.logs.clear()
        second = self._open()
        self.assertEqual(second.cached_exit, 0)
        self.assertTrue(any(line.startswith("baseline cache: reused key ")
                            for line in self.logs))
        self.assertTrue((self.dest / "final_link.o").is_file())
        key = _key(self.repo)[0]
        self.assertIn(key, self.logs[0])

    def test_different_inputs_do_not_reuse(self):
        first = self._open()
        self._build(first, elf=b"ELF-A")
        (self.repo / "include" / "a.h").write_text("#pragma once\n/*x*/\n",
                                                   encoding="utf-8", newline="\n")
        second = self._open()
        self.assertIsNone(second.cached_exit)
        self._build(second, elf=b"ELF-B")
        self.assertEqual((self.dest / "final_link.o").read_bytes(), b"ELF-B")

    def test_disabled_does_not_read_or_write_the_cache(self):
        seeded = self._open()
        self._build(seeded, elf=b"STORED")
        stored = (BC.BaselineStore(self.cache).entry_dir(_key(self.repo)[0])
                  / "final_link.o").read_bytes()
        self.logs.clear()
        if self.dest.exists():
            shutil.rmtree(self.dest)
        session = self._open(enabled=False)
        self.assertIsNone(session.cached_exit)
        self.assertTrue(any("--no-baseline-cache" in line for line in self.logs))
        self.assertFalse(self.dest.exists())
        session.finish(0)
        self.assertEqual(
            (BC.BaselineStore(self.cache).entry_dir(_key(self.repo)[0])
             / "final_link.o").read_bytes(),
            stored)

    def test_rebuild_replaces_and_a_failed_build_does_not(self):
        first = self._open()
        self._build(first, elf=b"OLD")
        self.logs.clear()
        again = self._open(rebuild=True)
        self.assertIsNone(again.cached_exit)
        self.assertTrue(any("--rebuild-baseline" in line for line in self.logs))
        self._build(again, elf=b"NEW", code=1)
        key = _key(self.repo)[0]
        self.assertEqual(
            (BC.BaselineStore(self.cache).entry_dir(key) / "final_link.o").read_bytes(),
            b"OLD")
        third = self._open(rebuild=True)
        self._build(third, elf=b"NEW", code=0)
        self.assertEqual(
            (BC.BaselineStore(self.cache).entry_dir(key) / "final_link.o").read_bytes(),
            b"NEW")

    def test_a_failed_content_check_does_not_reuse(self):
        first = self._open()
        self._build(first, elf=b"STORED")
        self.logs.clear()
        session = self._open(validate=lambda path: False)
        self.assertIsNone(session.cached_exit)
        self.assertTrue(any("failed content check" in line for line in self.logs))
        self._build(session, elf=b"FRESH", code=1)
        key = _key(self.repo)[0]
        self.assertEqual(
            (BC.BaselineStore(self.cache).entry_dir(key) / "final_link.o").read_bytes(),
            b"STORED")

    def test_verify_matches_and_verify_differs(self):
        first = self._open()
        self._build(first, elf=b"SAME")
        self.logs.clear()
        ok = self._open(verify=True)
        self.assertIsNone(ok.cached_exit)
        self.assertTrue(any("verifying key" in line for line in self.logs))
        code = self._build(ok, elf=b"SAME")
        self.assertEqual(code, 0)
        self.assertTrue(any("verify matches key" in line for line in self.logs))

        self.logs.clear()
        bad = self._open(verify=True)
        code = self._build(bad, elf=b"OTHER")
        self.assertEqual(code, 1)
        self.assertTrue(any("verify differs key" in line for line in self.logs))
        key = _key(self.repo)[0]
        self.assertEqual(
            (BC.BaselineStore(self.cache).entry_dir(key) / "final_link.o").read_bytes(),
            b"SAME")

    def test_verify_with_nothing_stored_exits_2_and_publishes(self):
        session = self._open(verify=True)
        code = self._build(session, elf=b"FIRST")
        self.assertEqual(code, 2)
        self.assertTrue(any("no previous entry" in line for line in self.logs))
        key = _key(self.repo)[0]
        self.assertTrue(BC.BaselineStore(self.cache).ready(key))

    def test_lock_timeout_computes_locally_without_publishing(self):
        key, _info = _key(self.repo)
        path = BC.BaselineStore(self.cache)._lock_path(key)
        path.parent.mkdir(parents=True)
        path.write_text(
            f"pid={os.getpid()}\nhost={socket.gethostname()}\n"
            f"created={time.time():.3f}\ntoken=held\n",
            encoding="utf-8", newline="\n")
        session = self._open(lock_timeout=0.25)
        self.assertIsNone(session.cached_exit)
        self.assertTrue(any("lock timeout" in line for line in self.logs))
        self._build(session, elf=b"LOCAL")
        self.assertTrue((self.dest / "final_link.o").is_file())
        self.assertFalse(BC.BaselineStore(self.cache).ready(key))

    def test_the_second_thread_waits_and_reuses(self):
        started = threading.Event()
        release = threading.Event()
        builds = []
        results = []

        def worker():
            session = self._open(lock_timeout=5)
            if session.cached_exit is not None:
                results.append("hit")
                return
            builds.append(1)
            if len(builds) == 1:
                started.set()
                self.assertTrue(release.wait(5))
            _write_control(self.dest, elf=b"ONCE")
            session.finish(0)
            results.append("build")

        first = threading.Thread(target=worker)
        second = threading.Thread(target=worker)
        first.start()
        self.assertTrue(started.wait(5))
        second.start()
        store = BC.BaselineStore(self.cache)
        deadline = time.time() + 5
        while time.time() < deadline and not list(self.cache.glob("locks/*")):
            time.sleep(0.02)
        locks = list((self.cache / "locks").iterdir())
        self.assertEqual(len(locks), 1)
        self.assertIsNone(store.try_acquire(locks[0].name, stale_s=10**9, grace_s=10**9))
        release.set()
        first.join(5)
        second.join(5)
        self.assertFalse(first.is_alive() or second.is_alive())
        self.assertEqual(builds, [1])
        self.assertIn("hit", results)

    def test_tu_restore_prefers_a_full_control_and_leaves_a_current_local_one(self):
        no_rom = self._open(build_rom=False)
        self._build(no_rom, elf=b"NOROM")
        missing = pathlib.Path(self.tmp.name) / "missing"
        logs = []
        restored = BC.restore_cached_baseline(
            self.repo, missing, cache_root=self.cache, validate=lambda path: True,
            log=logs.append)
        self.assertEqual(restored, _key(self.repo, build_rom=False)[0])
        self.assertEqual((missing / "final_link.o").read_bytes(), b"NOROM")

        full = self._open(build_rom=True)
        self._build(full, elf=b"FULL")
        shutil.rmtree(missing)
        logs.clear()
        restored = BC.restore_cached_baseline(
            self.repo, missing, cache_root=self.cache, validate=lambda path: True,
            log=logs.append)
        self.assertEqual(restored, _key(self.repo, build_rom=True)[0])
        self.assertTrue(any("restored local control" in line for line in logs))

        (missing / "final_link.o").write_bytes(b"LOCAL")
        logs.clear()
        self.assertIsNone(BC.restore_cached_baseline(
            self.repo, missing, cache_root=self.cache, validate=lambda path: True,
            log=logs.append))
        self.assertEqual((missing / "final_link.o").read_bytes(), b"LOCAL")
        self.assertEqual(logs, [])

    def test_key_cli_prints_the_content_key(self):
        out = io.StringIO()
        err = io.StringIO()
        with redirect_stdout(out), redirect_stderr(err):
            code = BC.main(["key", "--repo", str(self.repo)])
        self.assertEqual(code, 0)
        self.assertEqual(out.getvalue().strip(), _key(self.repo)[0])
        self.assertIn("inputs clean", err.getvalue())


class CurrentControlTests(unittest.TestCase):
    def test_a_report_without_evidence_is_not_current(self):
        import tubuild
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        dest = pathlib.Path(tmp.name) / "control"
        _write_control(dest)
        self.assertFalse(tubuild.baseline_dir_current(dest))
        report_path = dest / "linkcheck.json"
        import json
        report = json.loads(report_path.read_text(encoding="utf-8"))
        report["baseline"] = False
        report_path.write_text(json.dumps(report), encoding="utf-8")
        self.assertFalse(tubuild.baseline_dir_current(dest))


class PorcelainTests(unittest.TestCase):
    def test_rename_records_carry_both_paths(self):
        blob = b"R  src/old.c\0src/new.c\0?? src/extra.c\0"
        self.assertEqual(
            BC._parse_porcelain_z(blob), ["src/old.c", "src/new.c", "src/extra.c"])

    def test_a_torn_record_is_not_trusted(self):
        self.assertIsNone(BC._parse_porcelain_z(b" M src/a.c"))
