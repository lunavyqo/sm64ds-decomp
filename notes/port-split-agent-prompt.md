# Split the PC port out of tangosdev/sm64ds-decomp

You are doing the repository split. Tangosdev (Tango) will paste this prompt to you. He is not a GitHub expert. Follow it literally. When a step says STOP, stop and report the exact command output. Do not improvise a different history rewrite.

Upstream matching repo: https://github.com/tangosdev/sm64ds-decomp
New port repo: https://github.com/tangosdev/sm64ds-port
Do this only as GitHub user `tangosdev`. Do not push to `lunavyqo/sm64ds-decomp` or any other fork.

Numbers below were measured on 2026-09-23. Re-read every SHA and branch list from GitHub before you use it. If a live value disagrees with an example SHA in this prompt, the live value wins. The example SHAs are sanity checks, not pins you invent.

## Goal

Publish the PC port as its own repository, with its commit history kept, and then open one normal pull request on `tangosdev/sm64ds-decomp` that removes the in-tree `port/` directory from **decomp `main` only**.

Two different trees are both called `port/` today. Both must be published. They are not interchangeable.

1. **Smoke suite, on decomp `main`.** Top of `port/` is `CMakeLists.txt`, `README.md`, `build-port.cmd`, `host_abi.h`, `docs/`, `hal/` (about 20 files), `ntr/`, `tests/`, `tools/` (five scripts: `hostgen.py`, `host_frontier.py`, `linkage.py`, `ntr_manifest.py`, `romdata.py`), `unmatched/` (two `.cpp` files), and these manifests only: `slice_gate1.txt`, `slice_gate2.txt`, `slice_gate3a.txt`, `slice_gate3b.txt`, `slice_gate4b.txt`, `slice_gate6.txt`, `slice_gate7.txt`, `slice_gate8.txt`, `slice_gate9.txt`. This is what `.github/workflows/port-build.yml` builds. It is kept path-compatible with current matching `src/`. It is not the downloadable game.
2. **Playable port, on `port/*` and `port-*` branches.** `port/link100` is the integration branch named by `config/port_linkage.json`. On 2026-09-23 its tip was `c0a3e1da770bad822c040ceacf4850f3b7aeac1e`, 3515 commits ahead of decomp `main` and 682 behind (`gh api repos/tangosdev/sm64ds-decomp/compare/main...port/link100`). `port/release-0.4.0` was `6230b2996636e3fd0e03cb178161c94cb4b73cc9`. `hal/` there has hundreds of files (335 on that date), `CMakeLists.txt` is about 1.3 MB / 22,367 lines, and the tree also has `res/`, `third_party/stb/`, `tools/battery.py`, `tools/romblob.py`, `ov*_syms.txt`, and hundreds of `slice_*.txt` files. Tag `v0.3.3` (annotated, message starts `0.3.3: generic controllers`) points at a combined commit whose game half is this line. The download at https://tangos.dev/downloads is this line, not decomp `main`.

The new repo's default branch `main` is the filtered playable integration branch (`port/link100`, or whatever branch `config/port_linkage.json` field `branch` names when you read it). The filtered smoke suite is published as branch `decomp-main`. It is not thrown away and it is not merged into the playable tree.

## Success criteria

All of these are true before you report done:

- `https://github.com/tangosdev/sm64ds-port` exists, public, owned by `tangosdev`, and its default branch is `main`.
- `main` contains the playable tree at repo root (`hal/sub_screen.cpp`, `res/walk_window.rc`, `tools/battery.py`, `third_party/stb/stb_image.h`) and `git rev-list --count main` is well above the smoke suite (if the count is under 200, STOP; the playable history was not imported).
- `decomp-main` contains the smoke suite at repo root (`slice_gate1.txt`, `tools/hostgen.py`, `hal/shims.cpp`) and does not have to contain `hal/sub_screen.cpp`.
- Every `port/*` branch, every `port-*` branch, `ci/port-build-proto`, and tag `v0.3.3` that existed on the decomp at the start of the run also exist on the new repo. Missing any of them is a failed split.
- History is rewritten only inside the fresh mirror clone. Commit subjects survive. The gate-1 subject `port: gate 1 - decompiled code runs natively on PC` is reachable from `decomp-main`. A current `port/link100` subject is reachable from `main`.
- No commit was pushed to `tangosdev/sm64ds-decomp` except the cleanup branch you open as a pull request. Decomp `main` was not force-pushed. No `port/*` branch on the decomp was deleted.
- A normal (not force) push created the port repo. The first push was to an empty repository.
- The port repo contains no ROM, no `extracted/` dump, no `mwccarm`, no `license.dat`, no claims key, no `ghp_` / `github_pat_` token.
- `SPLIT.md`, `DECOMP-PINS.tsv`, `LICENSE`, `.gitignore`, and `tools/check_nest.py` are on both `main` and `decomp-main`.
- Smoke CI on the port repo's `decomp-main` configures and links the five ROM-free targets against a sparse checkout of decomp `main`.
- Playable CI on the port repo's `main` checks out the recorded decomp pin with `port/` excluded, nests this repo at `port/`, and `python tools/check_nest.py` exits 0.
- A pull request is open against `tangosdev/sm64ds-decomp` `main` that deletes `port/`, deletes `.github/workflows/port-build.yml`, gitignores `/port/`, makes `tools/port_refcheck.py` skip when `port/` is absent, and retargets living docs. That PR does not rewrite history. You do not merge it.

## How the build finds the decomp (do not redesign this)

Every current build script treats "the directory that contains `port/`" as the decomp root. That is the wiring. Keep it.

Measured on decomp `main` and confirmed on `port/link100`:

- `port/CMakeLists.txt` (smoke file, and the 22,367-line playable file) appends slice lines as `${CMAKE_CURRENT_SOURCE_DIR}/../${line}` and adds `${CMAKE_CURRENT_SOURCE_DIR}/../include`. The playable file sets `PORT_HOSTGEN_TU_ROOT` to `${CMAKE_CURRENT_SOURCE_DIR}/..` (line 1075 on `port/link100` as of 2026-09-23) and passes `--root ${CMAKE_CURRENT_SOURCE_DIR}/..` into `tools/facegen.py`.
- Slice lines are **decomp-root-relative**. Smoke examples: `src/DotVec3.c`, `port/unmatched/ModelComponents_Render.cpp`, `port/unmatched/MeshCollider_DetectClsn_RaycastLine.cpp`. Playable `slice_gate10.txt` is the same shape (a thousand `src/` lines plus a handful of `port/` lines). `ov*_syms.txt` files are symbol tables, not source paths.
- `port/tools/hostgen.py` defaults `--decomp` to `Path(__file__).resolve().parents[2]`, which is the decomp root when the script lives at `<decomp>/port/tools/hostgen.py`. CMake on `main` invokes it without `--decomp`. It then imports `<decomp>/tools/srcpath.py`.
- `port/tools/romdata.py` and `port/tools/ntr_manifest.py` use `parents[2]` and read `<decomp>/extracted/arm9_dec.bin` and `<decomp>/build/assets/files.tsv`. Those paths are gitignored outputs of the decomp's `tools/unpack.py` and `tools/asset_catalog.py`. They are not in git and they must not move into the port repo.
- `port/tools/host_frontier.py` sets `REPO = parents[2]`, `SRC = REPO/src`, `INCLUDE = REPO/include`, `PORT = REPO/port`.
- `port/build-port.cmd` refuses to run unless `%~dp0..\extracted\arm9_dec.bin`, `%~dp0..\build\assets\files.tsv`, and `handles.tsv` exist, then configures `-S %~dp0.` and `-B %~dp0..\build\port`.
- `PORT_REPO_ROOT` in the smoke `CMakeLists.txt` is the absolute parent of `port/`. `port/hal/fs.cpp` and several smokes use it as the asset root (overridable with env `SM64DS_ASSET_ROOT`). Assets stay in the decomp checkout.
- `port/host_abi.h` rejects a 64-bit host. The MSVC build is 32-bit (`vcvars32`), same as `port-build.yml`.

**Chosen wiring: strip the `port/` prefix, then clone the port repo into a decomp checkout at the directory named `port/`.** After the strip, the port repo root is today's `port/` contents. Nesting it at `<sm64ds-decomp>/port/` recreates the paths the scripts already use. `../src`, `../include`, `../tools/srcpath.py`, `../extracted`, and `../build/assets` resolve again. Slice text `port/unmatched/...` still resolves, because that string is decomp-root-relative and the nest recreates `<decomp>/port/unmatched/...`.

Do not rewrite `CMakeLists.txt`, `hostgen.py`, `romdata.py`, `build-port.cmd`, or the slice manifests to a sibling path or a `--decomp` flag. The playable `CMakeLists.txt` is over twenty thousand lines. A path rewrite there will break the game. `git-filter-repo --subdirectory-filter port` does the prefix strip. No source edit is required for the nest to work.

Why this and not the other two shapes:

- A **sibling clone** (`../sm64ds-decomp`) does not match `CMAKE_CURRENT_SOURCE_DIR/..` or `parents[2]`. Making it match means editing the 22k-line CMake and every script. Do not do that in this split.
- A **git submodule** either puts the decomp inside the port repo (`decomp/src` instead of `../src`) or puts a gitlink to the port back inside the matching repo. One pin also cannot serve both decomp `main` (smoke suite) and `port/link100` (playable line, hundreds of commits off main). Do not add a submodule.

The decomp checkout must **exclude its own tracked `port/`** before you nest the new repo, or the two trees collide. This was tested with:

```sh
git sparse-checkout init --no-cone
git sparse-checkout set '/*' '!/port/'
```

On a full decomp `main` checkout that still contains `port/`, that leaves `src/`, `include/`, and `tools/srcpath.py` present and removes `port/`. Use it for every decomp SHA, including `main` before the cleanup PR merges and including playable pins (those commits still track `port/`).

Playable `main` is built only against its recorded decomp pin. Smoke `decomp-main` is built against decomp `main`. Nesting the playable tree into a decomp `main` worktree will fail `port_refcheck` and the slice paths. That failure is expected. Do not "fix" it by editing slices during this split. Use two decomp worktrees.

## What moves, what stays

### Moves into tangosdev/sm64ds-port (via history filter of `port/` only)

The entire `port/` tree on every selected ref, prefix stripped so `port/hal/fs.cpp` becomes `hal/fs.cpp`. That includes, when present on that ref: `CMakeLists.txt`, `README.md`, `build-port.cmd`, `build-port-trace.cmd`, `host_abi.h`, `hal/`, `ntr/`, `tests/`, `tools/`, `unmatched/`, `docs/`, `res/`, `third_party/`, every `slice*.txt`, every `ov*_syms.txt`, and the rest of the playable top-level text files.

### Copied by hand, because they are port material that does not live under `port/`

- `.github/workflows/port-build.yml` from decomp `main`. It is deleted from the decomp in the cleanup PR and recreated on the port repo's `decomp-main` branch only, using the workflow later in this prompt. `git-filter-repo --subdirectory-filter port` will not carry it.
- `notes/port-selftest-bmp-gate.md` from decomp `main`. Copy it to `docs/port-selftest-bmp-gate.md` on the port repo's `main` (the playable tree, which actually has `tools/battery.py`, `hal/host_settings.cpp`, and `ov009_syms.txt`). In the copy, rewrite the path prefixes `port/tools/` → `tools/`, `port/hal/` → `hal/`, `port/ov` → `ov` so they match the stripped tree. Then delete the note from the decomp in the cleanup PR.

### Stays in the decomp

- All of `src/` and `include/`, including every `#ifndef SM64DS_PLATFORM_PC` guard. Those guards are how the matching build keeps mwccarm `sizeof` asserts while the host build defines `SM64DS_PLATFORM_PC`. They are matching source. Do not delete them, do not move them, and do not edit them in the cleanup PR.
- `tools/srcpath.py`, `tools/unpack.py`, `tools/asset_catalog.py`, `config/**`, the matching workflows, `tools/hooks/pre-push` (edited only to describe the skip), `tools/port_refcheck.py` and `tools/test_port_refcheck.py` (the checker stays; it learns to skip when `port/` is absent), `tools/cpp_tu_compat.py` (it imports `port_refcheck` and must keep compiling), `tools/validate_merge.py`'s optional `--port-refcheck-report` (absent report is already neutral), `config/port_linkage.json` (the LINKED stamp; update the comment that tells a human how to refresh it, do not change the numbers).
- Decomp `port/*`, `port-*`, and `ci/port-build-proto` branches, and tag `v0.3.3`. They remain the combined snapshot (decomp `src/` at that commit plus the old in-tree `port/`). Deleting them destroys the only tree the playable pin can build against. Do not delete them.

### Does not move

- `src/` edits that exist only on a `port/*` branch. `--subdirectory-filter port` drops them from the port repo on purpose. They stay on that decomp branch. Building that port branch means checking out that same decomp SHA. Do not also filter `src/` or `include/` into the port repo.
- `mods/Player_ScaleByCharFactor.c` (present on the `v0.3.3` tag, not part of `port/`).
- Matching branches (`cpp/*`, `agents/*`, `fix/*`, and the rest). Re-list refs; do not include a branch just because its name contains the substring `port` (`docs/profile-lifecycle-crosswalk` is not a port branch).

## Hard rules

- Do not run `git filter-repo` inside a decomp checkout you use for matching. Use a new directory.
- Do not pass `--force` to `git filter-repo` to override its fresh-clone refusal. If it demands `--force`, you are in the wrong repository. STOP.
- Do not force-push. Do not `--force`, `--force-with-lease`, or `--mirror` push. A brand-new empty repo accepts a normal push.
- Do not push the filtered history to `tangosdev/sm64ds-decomp`. Before every push, the destination URL must contain `tangosdev/sm64ds-port` and must not contain `sm64ds-decomp`.
- Do not create the port repo under any account but `tangosdev`. If the name is taken, or the repo exists and is not empty, STOP and ask Tango. Do not delete a repository.
- Do not commit a ROM, an `extracted/` tree, `mwccarm`, `license.dat`, a claims key, or a token. `res/walk_window.ico` on `port/link100` is a plain window glyph (a dark rounded square and a lighter pane). Keep that file. Anything that is a character sprite, a level texture, a `.nds`, a `.bmd`, or a `romdata.bin` does not get pushed. If such a blob is in history, strip it with a second `git filter-repo` **before the first push**. After the first push, do not rewrite history; delete the file in a new commit only if it is on the tip and was not in older commits you already pushed. If you already pushed a bad blob, STOP and tell Tango. Do not force-push the fix.
- Do not merge the playable tree and the smoke suite together.
- Do not edit the playable `CMakeLists.txt` to change path resolution.
- Do not delete decomp `port/*` branches.
- Do not touch decomp `src/` or `include/` in the cleanup PR.
- The word the matching repo uses for sloppy history is irrelevant here. Do not go hunting for a style rewrite. This task is the split.

## Procedure

### 0. Identity, tools, and the name

```sh
gh auth status
```

The active account must be `tangosdev`. If it is not, STOP. Do not `gh auth login` as someone else.

```sh
gh repo view tangosdev/sm64ds-port --json name,isEmpty,url
```

A 404 means the name is free for this token. Confirm the token can see private repos (`repo` scope) so a private existing repo is not invisible. If the repo exists and `isEmpty` is false, STOP and ask Tango. If it exists and is empty, you may push to it. If it does not exist, create it only after the local filter is scanned and ready, with:

```sh
gh repo create tangosdev/sm64ds-port --public --disable-wiki \
  --description "Native PC port of Super Mario 64 DS. Byte-matching source stays in tangosdev/sm64ds-decomp."
```

Do not pass `--clone` and do not let `gh` add a README or a license. The first commit must be the filtered history, not an empty GitHub commit you then have to merge.

Install the rewriter if needed and read its help before you use it:

```sh
python3 -m pip install --user git-filter-repo
git filter-repo -h | head -n 80
```

You need `--subdirectory-filter` and `--refs` in that help. If they are absent, STOP. Do not fall back to `git filter-branch`.

### 1. Fresh mirror, never the matching worktree

Pick a new directory. This example uses `$HOME/sm64ds-port-split`. Do not reuse a decomp clone.

```sh
mkdir -p "$HOME/sm64ds-port-split"
cd "$HOME/sm64ds-port-split"
git clone --mirror https://github.com/tangosdev/sm64ds-decomp.git decomp-mirror.git
cd decomp-mirror.git
git remote get-url origin
git rev-parse --is-bare-repository
```

Origin must be `https://github.com/tangosdev/sm64ds-decomp.git` (or the SSH form of that same repo). `--is-bare-repository` must print `true`. If origin is a fork, STOP.

Re-list the refs you will keep. Include a branch only when its name is exactly one of these:

- `main`
- `port/<anything>`
- `port-<anything>` (examples that existed on 2026-09-23: `port-0.2.7-hotfix`, `port-fix-ledgehang`, `port-fix-momentum`, `port-fix-walljump`, `port-gate-10`, `port-infra-consolidated`, `port-mount-noseat-cluster`, `port-particles`, `port-portable-kit`, `port-stream-b`)
- `ci/port-build-proto`
- tag `v0.3.3`

```sh
git for-each-ref --format='%(objectname) %(refname)' \
  refs/heads/main \
  'refs/heads/port/*' \
  'refs/heads/port-*' \
  refs/heads/ci/port-build-proto \
  refs/tags/v0.3.3 | tee "$HOME/sm64ds-port-split/pins-before.txt"
```

Confirm `refs/heads/port/link100` (or the branch named by a fresh read of decomp `config/port_linkage.json` key `branch`) is in that file. Confirm `refs/heads/port/release-0.4.0` and `refs/tags/v0.3.3` are in it. If `port/link100` is gone, read `config/port_linkage.json` from decomp `main` (`git show main:config/port_linkage.json`) and use the `branch` value. If that branch does not exist either, STOP and ask Tango which playable branch is the default.

Count them. On 2026-09-23 there were 56 `port/*` branches, 10 `port-*` branches, plus `main`, `ci/port-build-proto`, and `v0.3.3`. A live count that is far smaller means the mirror is incomplete. STOP.

Also print the integration tip and decomp `main` tip, to a different file so they do not get mixed into the `SHA ref` list:

```sh
git rev-parse refs/heads/main refs/heads/port/link100 | tee "$HOME/sm64ds-port-split/tip-shas.txt"
```

### 2. Filter

From inside the mirror:

```sh
mapfile -t REFS < <(git for-each-ref --format='%(refname)' \
  refs/heads/main \
  'refs/heads/port/*' \
  'refs/heads/port-*' \
  refs/heads/ci/port-build-proto \
  refs/tags/v0.3.3)

args=()
for r in "${REFS[@]}"; do
  args+=(--refs "$r")
done

git filter-repo --subdirectory-filter port "${args[@]}"
```

`--subdirectory-filter port` keeps only history that touches `port/` and makes that directory the repository root. Do not also pass `--path-rename`. Do not pass `--invert-paths`.

`git filter-repo` removes `origin`. Confirm that:

```sh
git remote -v
```

The command must print nothing. If `origin` still points at `sm64ds-decomp`, do not push. Remove it with `git remote remove origin` only if you are still inside this mirror and you have not pushed.

Write the branch map the docs commit will ship:

```sh
{
  echo -e "ref\tdecomp_sha\tport_sha"
  while read -r old ref; do
    new=$(git rev-parse "$ref")
    printf '%s\t%s\t%s\n' "$ref" "$old" "$new"
  done < "$HOME/sm64ds-port-split/pins-before.txt"
} | tee "$HOME/sm64ds-port-split/DECOMP-PINS.tsv"
```

`pins-before.txt` is only the `for-each-ref` output: each line is `SHA ref`. Do not append anything else to that file. Every `decomp_sha` is the pre-filter SHA. Every `port_sha` is the post-filter SHA. They must differ.

### 3. Branch names

Still in the mirror:

```sh
git branch -m main decomp-main
git branch main refs/heads/port/link100
git symbolic-ref HEAD refs/heads/main
```

If the integration branch is not `port/link100`, use the live name in that second command. `main` and `port/link100` should point at the same filtered commit before the docs commit. `decomp-main` is the filtered decomp `main`. Do not delete `port/link100`.

Sanity, still in the mirror:

```sh
git ls-tree -r --name-only main | grep -c .
git cat-file -e main:hal/sub_screen.cpp
git cat-file -e main:res/walk_window.rc
git cat-file -e main:tools/battery.py
git cat-file -e decomp-main:slice_gate1.txt
git cat-file -e decomp-main:tools/hostgen.py
git cat-file -e decomp-main:hal/shims.cpp
git log -1 --format='%s' decomp-main
git merge-base --is-ancestor $(git rev-list --max-count=1 --grep='port: gate 1' decomp-main) decomp-main
```

`git cat-file -e` exits non-zero when the path is missing. Any miss here means the filter or the branch rename is wrong. STOP.

`git rev-list --count main` under 200 means you filtered only the smoke history. STOP.

### 4. Docs commits on a worktree

```sh
# A bare repo treats HEAD as a checkout. Point it at a ref you will not
# edit, or `git worktree add` of main/decomp-main fails with "already checked out".
git symbolic-ref HEAD refs/heads/ci/port-build-proto
git worktree add "$HOME/sm64ds-port-split/wt-main" main
cd "$HOME/sm64ds-port-split/wt-main"
```

Copy `LICENSE` byte-for-byte from decomp `main` (`git --git-dir=$HOME/sm64ds-port-split/decomp-mirror.git show decomp-main:LICENSE` will fail, because `LICENSE` lived at the decomp root and the filter dropped it). Take it from the mirror **before** you need it: if you already filtered, fetch the blob from GitHub:

```sh
gh api repos/tangosdev/sm64ds-decomp/contents/LICENSE?ref=main --jq .content | base64 -d > LICENSE
```

Confirm the file starts with `MIT License` and contains `Copyright (c) 2026 Tango`. Do not edit it.

Write `.gitignore` with exactly this content:

```
# Nintendo-derived and local build output. Never commit these.
build/
extracted/
frames/
playlog/
*.bmp
*.nds
*.bin
*.sav
*.mln
*.ml1
*.ml2
*.ml3
*.ml4
*.ml5
*.ml6
*.ml7
*.ml8
*.ml9
*.ml10
baserom*
mwccarm.zip
tools/mwccarm/
license.dat
__pycache__/
*.pyc
.vs/
```

Write `tools/check_nest.py` with exactly this content:

```python
#!/usr/bin/env python3
"""Fail unless this repo is nested at <sm64ds-decomp>/port and slice paths resolve.

Slice manifests are decomp-root-relative (src/..., port/unmatched/...).
ov*_syms.txt is a symbol table, not a path list, and is not read here.
"""
import pathlib
import sys

PORT = pathlib.Path(__file__).resolve().parents[1]
DECOMP = PORT.parent


def main():
    if PORT.name != "port":
        sys.exit(f"this repo is at {PORT}, and the directory must be named port")
    if not (DECOMP / "src").is_dir() or not (DECOMP / "tools" / "srcpath.py").is_file():
        sys.exit(
            f"{DECOMP} is not an sm64ds-decomp checkout "
            "(need src/ and tools/srcpath.py). See SPLIT.md."
        )
    if not (DECOMP / "include").is_dir():
        sys.exit(f"missing {DECOMP / 'include'}")
    missing = []
    manifests = sorted(PORT.glob("slice*.txt"))
    if not manifests:
        sys.exit("no slice*.txt manifests in the port repo root")
    for manifest in manifests:
        text = manifest.read_text(encoding="utf-8", errors="replace")
        if text.startswith("\ufeff"):
            text = text.lstrip("\ufeff")
        for lineno, raw in enumerate(text.splitlines(), 1):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            rel = line.split()[0]
            if not (DECOMP / rel).exists():
                missing.append(f"{manifest.name}:{lineno}: {rel}")
    if missing:
        print("\n".join(missing[:40]))
        sys.exit(f"{len(missing)} slice path(s) missing under {DECOMP}")
    print(f"nest ok: {len(manifests)} manifests, decomp {DECOMP}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
```

Copy `DECOMP-PINS.tsv` from `$HOME/sm64ds-port-split/DECOMP-PINS.tsv` into the worktree root.

Write `SPLIT.md` with the following facts, in this order. Use these words so a later reader does not invent a sibling layout:

1. This repository is the old `port/` directory of https://github.com/tangosdev/sm64ds-decomp with the `port/` prefix removed. Byte-matching source stays in the decomp repo.
2. Clone the decomp, exclude its tracked `port/`, then clone this repo into that checkout at a directory that must be named `port`:

```sh
git clone https://github.com/tangosdev/sm64ds-decomp.git sm64ds-decomp
cd sm64ds-decomp
git checkout <decomp-pin-or-main>
git sparse-checkout init --no-cone
git sparse-checkout set '/*' '!/port/'
test ! -e port
test -f tools/srcpath.py
git clone https://github.com/tangosdev/sm64ds-port.git port
cd port
git checkout <port-branch>
python tools/check_nest.py
```

3. Which pin. Branch `main` of this repo (and `port/link100`) builds against the `decomp_sha` for `refs/heads/port/link100` in `DECOMP-PINS.tsv`. Branch `decomp-main` of this repo builds against decomp branch `main`. Do not point the playable branch at decomp `main`. Do not point the smoke branch at the playable pin.
4. Use a second decomp worktree for matching work. Do not nest the playable `port/` inside the decomp `main` worktree you push matching commits from. `tools/port_refcheck.py` in the decomp still runs when `port/` exists, and the playable slices do not match decomp `main`'s `src/`.
5. Assets stay in the decomp checkout. From the decomp root, with your own cartridge dump: `python tools/unpack.py path/to/your-own-sm64ds.nds` then `python tools/asset_catalog.py generate path/to/your-own-sm64ds.nds`. Then, on Windows with the 32-bit MSVC toolset, run `port\build-port.cmd`. The ROM, `extracted/`, and `build/assets/` are gitignored. Do not commit them.
6. `hostgen.py`, `romdata.py`, and `build-port.cmd` find the decomp as the parent of this repo. That is correct only while this repo is checked out at `<decomp>/port`. Passing `--decomp` is for a one-off `hostgen.py` invocation from another directory. Do not change the default to a sibling path.
7. Legal. This repo is MIT, same license file as the decomp, and that license covers the original code only. It contains no ROM and no Nintendo assets. Do not add any. `third_party/stb/` is the public-domain stb headers and stays. `res/walk_window.ico` is a plain window icon and stays.
8. After the split, a rename in decomp `src/` can strand a slice path. Fix that in this repo, on `decomp-main` for the smoke suite or on the playable branch when the pin moves. The decomp cleanup no longer carries `port/` on `main`.
9. SHAs in this repo are not decomp SHAs. `DECOMP-PINS.tsv` maps them. `git filter-repo` rewrote every published commit.

Commit on `main`:

```sh
git add LICENSE .gitignore SPLIT.md DECOMP-PINS.tsv tools/check_nest.py
git commit -m "Record how this repo nests back into sm64ds-decomp"
```

Copy the playable selftest note onto this same branch:

```sh
gh api repos/tangosdev/sm64ds-decomp/contents/notes/port-selftest-bmp-gate.md?ref=main --jq .content \
  | base64 -d > docs/port-selftest-bmp-gate.md
```

Then replace the path prefixes in that copy only (`port/tools/` → `tools/`, `port/hal/` → `hal/`, `port/ov` → `ov`). Commit:

```sh
git add docs/port-selftest-bmp-gate.md
git commit -m "Move the walk_window selftest note into the port repo"
```

Add the playable nest workflow `.github/workflows/nest-check.yml` on `main` only, with exactly this content. `DECOMP_REF` is read from the pin file so you do not hard-code a SHA that this prompt's examples may have stale:

```yaml
name: nest-check

on:
  pull_request:
    branches: [main]
  push:
    branches: [main]
  workflow_dispatch:

permissions:
  contents: read

concurrency:
  group: nest-check-${{ github.event.pull_request.head.sha || github.ref }}
  cancel-in-progress: true

jobs:
  slices:
    runs-on: ubuntu-latest
    steps:
      - name: Port repo
        uses: actions/checkout@v4
        with:
          path: port-src

      - name: Pinned decomp, with its own port/ excluded
        uses: actions/checkout@v4
        with:
          repository: tangosdev/sm64ds-decomp
          ref: main
          path: decomp
          fetch-depth: 1

      - name: Check out the recorded pin and nest this repo at decomp/port
        run: |
          set -eu
          PIN=$(awk -F '\t' '$1=="refs/heads/port/link100" {print $2}' port-src/DECOMP-PINS.tsv)
          test -n "$PIN"
          git -C decomp fetch --depth 1 origin "$PIN"
          git -C decomp checkout "$PIN"
          git -C decomp sparse-checkout init --no-cone
          git -C decomp sparse-checkout set '/*' '!/port/'
          test ! -e decomp/port
          test -f decomp/tools/srcpath.py
          rm -rf decomp/port
          cp -a port-src decomp/port
          python decomp/port/tools/check_nest.py
```

If the integration branch is not `refs/heads/port/link100`, change that awk string to the live ref. Commit this workflow on `main`:

```sh
git add .github/workflows/nest-check.yml
git commit -m "Check playable slice paths against the recorded decomp pin"
```

Cherry-pick only the license / gitignore / SPLIT / pins / check_nest commit onto `decomp-main`. Do not cherry-pick the selftest note (those paths exist on the playable tree) and do not cherry-pick `nest-check.yml` (it pins the playable SHA).

```sh
DOCS=$(git log --format='%H' --grep='^Record how this repo nests back into sm64ds-decomp$' -n 1 main)
test -n "$DOCS"
git --git-dir="$HOME/sm64ds-port-split/decomp-mirror.git" symbolic-ref HEAD refs/heads/ci/port-build-proto
git worktree add "$HOME/sm64ds-port-split/wt-smoke" decomp-main
git -C "$HOME/sm64ds-port-split/wt-smoke" cherry-pick "$DOCS"
```

If the cherry-pick conflicts, STOP. Do not resolve it by taking the playable `CMakeLists.txt`.

On `wt-smoke`, add `.github/workflows/port-build.yml` with exactly this content, then commit it with subject `Run the smoke port build against decomp main`:

```yaml
name: port-build

on:
  pull_request:
    branches: [decomp-main]
  push:
    branches: [decomp-main]
  workflow_dispatch:

permissions:
  contents: read

concurrency:
  group: port-build-${{ github.event.pull_request.head.sha || github.ref }}
  cancel-in-progress: true

jobs:
  refs:
    runs-on: ubuntu-latest
    steps:
      - name: Port repo (this branch)
        uses: actions/checkout@v4
        with:
          path: port-src

      - name: Decomp main
        uses: actions/checkout@v4
        with:
          repository: tangosdev/sm64ds-decomp
          ref: main
          path: decomp
          fetch-depth: 1

      - uses: actions/setup-python@v5
        with:
          python-version: "3.12"

      - name: Exclude the decomp's tracked port/ and nest this repo there
        run: |
          set -eu
          git -C decomp sparse-checkout init --no-cone
          git -C decomp sparse-checkout set '/*' '!/port/'
          test ! -e decomp/port
          test -f decomp/tools/srcpath.py
          rm -rf decomp/port
          cp -a port-src decomp/port
          python decomp/port/tools/check_nest.py
          mkdir -p decomp/build
          python decomp/tools/port_refcheck.py --json decomp/build/port_refcheck.json

  build:
    runs-on: windows-latest
    steps:
      - name: Port repo (this branch)
        uses: actions/checkout@v4
        with:
          path: port-src

      - name: Decomp main
        uses: actions/checkout@v4
        with:
          repository: tangosdev/sm64ds-decomp
          ref: main
          path: decomp
          fetch-depth: 1

      - uses: actions/setup-python@v5
        with:
          python-version: "3.12"

      - name: Ninja
        run: python -m pip install --disable-pip-version-check ninja

      - name: Nest and configure the ROM-free targets
        shell: cmd
        run: |
          git -C decomp sparse-checkout init --no-cone
          git -C decomp sparse-checkout set "/*" "!/port/"
          if exist decomp\port (echo ERROR: sparse-checkout left port\ in place 1>&2 & exit /b 1)
          if not exist decomp\tools\srcpath.py (echo ERROR: srcpath.py missing 1>&2 & exit /b 1)
          rmdir /s /q decomp\port 2>nul
          xcopy /E /I /Q port-src decomp\port
          set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
          if not exist "%VSWHERE%" (echo ERROR: vswhere.exe not found 1>&2 & exit /b 1)
          set "VSINSTALL="
          for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
          if not defined VSINSTALL (echo ERROR: no VS install with the x86 C++ tools 1>&2 & exit /b 1)
          >>"%GITHUB_ENV%" echo VSINSTALL=%VSINSTALL%
          call "%VSINSTALL%\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 1
          cmake -S decomp\port -B decomp\build\portci -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

      - name: Link the ROM-free targets
        shell: cmd
        run: |
          call "%VSINSTALL%\VC\Auxiliary\Build\vcvars32.bat" >nul || exit /b 1
          cmake --build decomp\build\portci --target smoke smoke_gx smoke_heap smoke_roots smoke_fs -- -k 0
```

Until the decomp cleanup PR merges, decomp `main` still contains `tools/port_refcheck.py` and a tracked `port/`. The sparse-checkout removes the tracked tree and the nest puts the filtered smoke suite back. `port_refcheck.py` on current decomp `main` understands the nine `slice_gate` manifests. That is the right checker for `decomp-main`. Do not point this workflow at the playable pin.

The five targets are the ones the old workflow could link without a ROM: `smoke`, `smoke_gx`, `smoke_heap`, `smoke_roots`, `smoke_fs`. Do not add `smoke_model`, `smoke_fs`'s asset-backed cousins, or any target that runs `romdata.py`. `BUILD_TESTING` stays off. No cartridge is fetched.

`git -C` and `xcopy` on `windows-latest` are available. If `git sparse-checkout` from `cmd` mis-parses the quotes, run that step with `shell: bash` and leave only the `vcvars32` / `cmake` lines in `cmd`.

### 5. Scan before any push

From `wt-main`, and again with `--all` so every branch is covered:

```sh
git fetch "$HOME/sm64ds-port-split/decomp-mirror.git" \
  'refs/heads/*:refs/heads/*' 'refs/tags/*:refs/tags/*'

git ls-tree -r --name-only --all \
  | grep -Ei '\.(nds|sav|ml[0-9]+|bmd|nsbmd|arc)$|(^|/)(mwccarm|license\.dat|claims_key|romdata\.bin)(/|$)' \
  || true

git rev-list --all --objects \
  | git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' \
  | awk '$1=="blob" && $3+0 > 1000000 {print}'
```

Inspect every hit. A source file over 1 MB may be the playable `CMakeLists.txt` (about 1.3 MB). That file is allowed. A `.bin`, `.nds`, texture dump, or archive is not. `git log --all -S 'ghp_' --oneline` and the same for `github_pat_` and `AKIA` must be empty. If a secret is in history, STOP before the push and tell Tango. Do not print the secret in the report.

Also:

```sh
git grep -n -I -E 'ghp_|github_pat_|AKIA' $(git rev-parse --all) || true
```

If `git grep` refuses that many revs, use `git log --all -S` as above. An empty result is what you want.

### 6. Create the GitHub repo and push

Create the empty repo now if you did not in step 0. Then push with an explicit URL. Do not add a remote that points at the decomp.

```sh
URL=https://github.com/tangosdev/sm64ds-port.git
case "$URL" in
  *sm64ds-port*) ;;
  *) echo "refusing to push to $URL"; exit 1 ;;
esac
case "$URL" in
  *sm64ds-decomp*) echo "refusing to push to the decomp"; exit 1 ;;
esac

cd "$HOME/sm64ds-port-split/decomp-mirror.git"
git push "$URL" main
git push "$URL" decomp-main
git push "$URL" 'refs/heads/port/*:refs/heads/port/*'
git push "$URL" 'refs/heads/port-*:refs/heads/port-*'
git push "$URL" ci/port-build-proto
git push "$URL" v0.3.3
```

The docs commits were made on the worktrees, which use the mirror as their object store, so those tips are in the mirror. Confirm `git rev-parse main` in the mirror is the workflow commit, not the raw filtered tip, before you push.

If a push is rejected because the remote is not empty, STOP. Do not force-push.

Set the default branch:

```sh
gh repo edit tangosdev/sm64ds-port --default-branch main
```

### 7. Verify the GitHub repo

```sh
gh repo view tangosdev/sm64ds-port --json defaultBranchRef,url --jq '{branch:.defaultBranchRef.name,url:.url}'
```

`branch` must be `main`.

Clone a fresh copy to a third directory and check:

```sh
git clone https://github.com/tangosdev/sm64ds-port.git "$HOME/sm64ds-port-split/verify"
cd "$HOME/sm64ds-port-split/verify"
git rev-list --count main
git log --oneline --grep 'port: gate 1' decomp-main | head
git cat-file -e main:hal/sub_screen.cpp
git cat-file -e main:SPLIT.md
git cat-file -e main:LICENSE
git cat-file -e decomp-main:tools/check_nest.py
git cat-file -e decomp-main:.github/workflows/port-build.yml
git ls-remote --heads origin | grep -c 'refs/heads/port/'
git ls-remote --tags origin | grep v0.3.3
```

The `port/` head count must match the pre-filter `port/*` count. `v0.3.3` must exist. `git rev-list --count main` must be the large number from step 3, plus the docs commits.

Local nest check, smoke suite against decomp `main` (Linux, no MSVC required for this part):

```sh
git clone --filter=blob:none https://github.com/tangosdev/sm64ds-decomp.git "$HOME/sm64ds-port-split/nest-smoke"
cd "$HOME/sm64ds-port-split/nest-smoke"
git sparse-checkout init --no-cone
git sparse-checkout set '/*' '!/port/'
git clone https://github.com/tangosdev/sm64ds-port.git port
git -C port checkout decomp-main
python port/tools/check_nest.py
python tools/port_refcheck.py
```

`check_nest.py` must print `nest ok`. `port_refcheck.py` must print `all references resolve`. If the cleanup PR has already changed `port_refcheck.py` to skip when `port/` is missing, this still runs the real checks because `port/` exists.

Repeat the nest for the playable pin. Check out the `decomp_sha` from `DECOMP-PINS.tsv` for `refs/heads/port/link100`, sparse-exclude `port/`, clone the port repo, check out `main`, run `python port/tools/check_nest.py`. That must pass. Do not run the playable `port_refcheck.py` from a decomp `main` checkout.

The MSVC link of `smoke`, `smoke_gx`, `smoke_heap`, `smoke_roots`, and `smoke_fs` is the `port-build` workflow on `decomp-main`. If you have a Windows VM with the x86 toolset, run that configure and build locally too. If you do not, say so in the report and let GitHub's `windows-latest` job be the proof. Do not fetch a ROM to make the other smokes run.

### 8. Decomp cleanup PR

Do this only after the port repo push and the nest checks above have succeeded. If the port repo is missing history, do not delete `port/` from the decomp.

```sh
git clone https://github.com/tangosdev/sm64ds-decomp.git "$HOME/sm64ds-port-split/decomp-cleanup"
cd "$HOME/sm64ds-port-split/decomp-cleanup"
git checkout -b split/remove-port-from-decomp origin/main
git remote get-url origin
```

Origin must be `tangosdev/sm64ds-decomp`. If it is a fork, STOP and re-clone the upstream.

Delete the tracked tree and the workflow:

```sh
git rm -r port
git rm .github/workflows/port-build.yml
git rm notes/port-selftest-bmp-gate.md
```

Append this block to `.gitignore`:

```
# PC port. Separate repository: https://github.com/tangosdev/sm64ds-port
# Clone it into this directory (the directory name must stay `port`).
/port/
```

Edit `tools/port_refcheck.py` function `main`, before `report = build_report()`:

```python
    if not PORT.is_dir():
        print("port-refcheck: skipped (port/ not checked out; "
              "the PC port is https://github.com/tangosdev/sm64ds-port)")
        return 0
```

Do not change `check_hal_links` or the unit tests' fixtures. Those tests call the check functions directly and create their own `port/` directory. `python -m unittest tools.test_port_refcheck tools.test_cpp_tu_compat` must still pass. With `port/` deleted in this worktree, `python tools/port_refcheck.py` must exit 0 and print `skipped`.

Update the comment block at the top of `tools/hooks/pre-push` and the echo on the port check so they say the check skips when `port/` is not checked out, and that a playable nest does not belong in a decomp `main` worktree.

Update `config/port_linkage.json` `_comment` only. The refresh command becomes: check out the port repo at `port/` inside the decomp worktree that matches the stamp's branch, build that tree, then from the decomp root run `python port/tools/linkage.py .`. Do not change `linkedTus`, `matchedTus`, `branch`, `commit`, or `measuredAt`.

Retarget these living docs so they no longer link to a relative `port/` path. Point them at `https://github.com/tangosdev/sm64ds-port` and, where a file path is needed, at the stripped path on that repo (`https://github.com/tangosdev/sm64ds-port/blob/main/README.md` for the playable readme, `https://github.com/tangosdev/sm64ds-port/blob/decomp-main/README.md` for the smoke readme). Tell matching contributors:

- `src/` platform guards stay here.
- Slice manifests and `hal/` bridges now live in `sm64ds-port`.
- A rename that strands a smoke slice is fixed on `sm64ds-port` branch `decomp-main`.
- `python tools/port_refcheck.py` still runs when a port checkout is nested at `port/`; it skips when it is not.
- Do not nest the playable branch into the worktree you use for matching pushes.

Files to edit for that (they exist on decomp `main`):

- `README.md` (the LINKED paragraph and the `port/` link; keep the download link `https://tangos.dev/downloads`)
- `AGENTS.md` (the `port/` references section)
- `MERGE.md` (the `port/` reference-break paragraph)
- `notes/pr-validation.md`
- `notes/runbook-reference-repair.md`
- `notes/real-cpp-migration-runbook.md`
- `notes/agents/references/writer-v1.md`
- `notes/agents/references/pipeline-v1.md`
- `notes/source-review-observations.md` (the relative links to `../port/README.md`, `hostgen.py`, and `host_frontier.py`)
- `tools/tiers.py` (the comment that says LINKED reads `port/tools/linkage.py`)
- `tools/tubuild.py` (the print that tells the user to edit `port/CMakeLists.txt`)

Leave historical handoffs, `notes/collision-system.md`, `notes/ctor-migration.md`, `CLAIMS.md`, and `notes/system-provenance.md` alone unless `python tools/check_dead_references.py` reports a new failure in them. When it does, prefer a one-line URL retarget for a living instruction. For a sentence that is explicitly a historical `git show` of an old decomp branch (`origin/port-mount-noseat-cluster:port/unmatched/...`), keep the command, because those branches still exist on the decomp, and bank the dead path only if the checker requires it. Do not run `--update` or `--update-code` as a blanket refresh. A healed baseline row (a banked `port/` path whose citing file was deleted) is not a failure. Mention it in the PR. Do not bank a new dead link to hide a doc you should have retargeted.

`tools/cpp_tu_compat.py` keeps importing `port_refcheck`. Do not delete that surface.

Search the cleanup diff for `src/` and `include/` file changes. There should be none. If a comment in `src/` is the only way to clear a new dead-reference failure, reword the comment and do not change any other line, then say so in the PR. Do not run a matching rewrite.

Run:

```sh
python -m unittest tools.test_port_refcheck tools.test_cpp_tu_compat -v
python tools/port_refcheck.py
python tools/check_dead_references.py
```

`port_refcheck.py` must skip. `check_dead_references.py` must exit 0. If `port-build` was a required status check, list branch protection and rulesets:

```sh
gh api repos/tangosdev/sm64ds-decomp/branches/main/protection
gh api repos/tangosdev/sm64ds-decomp/rulesets
```

If a required check is the `port-build` workflow's jobs (`refs` and `build` under workflow name `port-build`), removing the workflow file makes that check never report, which blocks every future decomp PR. Remove only those required contexts. Do not disable `validate`, source review, or any other required check. If the API refuses the protection edit, STOP and put the exact check names in the report so Tango can remove them. Do not merge past a stuck required check.

Commit, push the branch with a normal `git push -u origin split/remove-port-from-decomp`, and open a PR into `main`. Title:

```
Move the PC port tree to tangosdev/sm64ds-port
```

PR body, filled in with the live SHAs and the nest-check output:

```
## What this changes

The PC port now lives at https://github.com/tangosdev/sm64ds-port.
This deletes the tracked `port/` tree from decomp main and retargets the docs
that pointed at it. Matching `src/` and `include/` are unchanged, including
the SM64DS_PLATFORM_PC sizeof guards.

The port repo's default branch `main` is the filtered `port/link100` history
(playable). Branch `decomp-main` there is the filtered smoke suite from this
repo's main. DECOMP-PINS.tsv in the port repo maps old decomp SHAs to new
port SHAs. Decomp `port/*` branches are intentionally still here; they are
the combined snapshot the playable pin builds against.

## Verification

- [ ] port repo push was a normal push to an empty tangosdev/sm64ds-port
- [ ] `git rev-list --count` on port `main` is the playable history, not the ~70-commit smoke history
- [ ] `python port/tools/check_nest.py` passed for decomp-main nested in decomp main
- [ ] `python port/tools/check_nest.py` passed for port main nested in the link100 pin
- [ ] `python tools/port_refcheck.py` on decomp main with the smoke nest passed before this deletion, and skips after it
- [ ] `python -m unittest tools.test_port_refcheck tools.test_cpp_tu_compat` passed
- [ ] `python tools/check_dead_references.py` passed
- [ ] no file under src/ or include/ is in this diff
- [ ] decomp main was not force-pushed
- [ ] port-build required check removed from branch protection, or it was not required

## Follow-ups that are not this PR

- Smoke-suite slice fixes after future src renames land on sm64ds-port branch decomp-main.
- Moving a playable pin forward onto newer decomp commits is separate work.
- Do not delete the decomp's port/* branches until each one has a pin in the port repo and Tango says the combined branch can go.
```

Do not merge the PR. Leave it for Tango once the checks he cares about are green.

## Mixed history, so you do not "clean it up"

On the decomp `main` that was current for this research (`9c6104f` on the lunavyqo fork, behind tangosdev `e04ccbc`), 73 commits touch `port/`. 20 of them touch only `port/`. Those are the 2026-08-02 gate commits starting at `9651d82a5172393584227a2190e34d3fde25a09b` (`port: gate 1`, PR #1001) through gate 9, plus a few later port-only follow-ups. The other 53 also change `src/`, `include/`, `config/`, and sometimes thousands of files. The port hunk in those commits is often a one-line slice path update inside a class rename or a TU promotion (`dActor_c` rename PR #1577 changed 6 files under `port/` and 2194 files elsewhere).

`git filter-repo --subdirectory-filter port` keeps those commits and drops the non-`port/` files. You will see famous rename subjects whose only remaining diff is a slice line. That is the correct result. Those slice updates are why the smoke suite still resolves against current `src/`. Do not drop mixed commits. Do not rebase them into one squashed "initial import".

Recompute the counts on the mirror before you filter if you want them in the cleanup PR body:

```sh
git rev-list main -- port | wc -l
```

and a `git diff-tree --name-only -r` per commit, counting paths that do not start with `port/`. Put the live counts in the report. Do not block the split on matching 53 and 73 exactly.

The playable branches are a second history. Filtering them keeps their `port/` files and drops `src/` edits that exist only on those branches. The decomp pin is what still has those `src/` edits. A filtered playable commit will not build against decomp `main`. Do not try to make it build by copying `src/` into the port repo.

## Rollback

- Before the first push to `sm64ds-port`: delete `$HOME/sm64ds-port-split` and stop. The decomp is untouched.
- After the port repo is pushed, before the cleanup PR merges: leave the port repo. Close the cleanup PR without merging. Decomp `main` still has `port/`.
- After the cleanup PR merges and something is wrong: open a normal revert PR of that merge. Do not force-push decomp `main`. The port repo stays; it is a copy.
- Do not delete `tangosdev/sm64ds-port` as a rollback. Ask Tango.

## Report back

When you stop, report:

- Port repo URL, default branch, `git rev-list --count` for `main` and `decomp-main`.
- The `decomp_sha` recorded for `refs/heads/port/link100` and for `refs/heads/main` (decomp main).
- How many `port/*`, `port-*`, and other refs were pushed, and whether `v0.3.3` was pushed.
- Nest-check output for both branches.
- Whether the five ROM-free targets linked, and if not, the CI URL.
- Whether the blob scan found anything you kept or stripped. Name the paths. Do not paste secret values.
- Cleanup PR URL, and the `git diff --stat` confirmation that `src/` and `include/` are absent.
- Branch-protection change, or the exact required-check names Tango still has to remove.
- Anything you skipped because a live SHA disagreed with this prompt.
