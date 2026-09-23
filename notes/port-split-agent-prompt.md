# Split the PC port out of tangosdev/sm64ds-decomp

Publish the PC port as https://github.com/tangosdev/sm64ds-port with history kept, then open one normal pull request on https://github.com/tangosdev/sm64ds-decomp that removes the in-tree `port/` tree from decomp `main` only.

The operator is GitHub user `tangosdev`. Every `gh` call and every push uses that account. Do not push to `lunavyqo/sm64ds-decomp` or any other fork.

Do not invent a different history rewrite. Measurements below are from 2026-09-23. Re-read every SHA, ref, and `config/port_linkage.json` before use. A live value overrides an example SHA.

## Outcome

Two different trees are both called `port/` today. Publish both. Do not merge them.

1. **Smoke suite, on decomp `main`.** `CMakeLists.txt`, `README.md`, `build-port.cmd`, `host_abi.h`, `docs/`, `hal/` (about 20 files), `ntr/`, `tests/`, `tools/` (`hostgen.py`, `host_frontier.py`, `linkage.py`, `ntr_manifest.py`, `romdata.py`), `unmatched/` (two `.cpp` files), and only `slice_gate1.txt`, `slice_gate2.txt`, `slice_gate3a.txt`, `slice_gate3b.txt`, `slice_gate4b.txt`, `slice_gate6.txt`, `slice_gate7.txt`, `slice_gate8.txt`, `slice_gate9.txt`. This is what `.github/workflows/port-build.yml` builds. It stays path-compatible with current matching `src/`. It is not the downloadable game.
2. **Playable port, on `port/*` and `port-*`.** The integration branch is the `branch` field of decomp `config/port_linkage.json` (`port/link100` on that date). Tip then: `c0a3e1da770bad822c040ceacf4850f3b7aeac1e` (3515 ahead of decomp `main`, 682 behind). `port/release-0.4.0` was `6230b2996636e3fd0e03cb178161c94cb4b73cc9`. That tree has hundreds of `hal/` files (335), a `CMakeLists.txt` of about 1.3 MB / 22,367 lines, `res/`, `third_party/stb/`, `tools/battery.py`, `tools/romblob.py`, `ov*_syms.txt`, and hundreds of `slice_*.txt`. Annotated tag `v0.3.3` (message starts `0.3.3: generic controllers`) points at a combined commit on this line. https://tangos.dev/downloads is this line.

New-repo `main` is the filtered playable integration branch. New-repo `decomp-main` is the filtered smoke suite.

### Success

- `tangosdev/sm64ds-port` is public, owned by `tangosdev`, default branch `main`.
- `main` has the playable tree at repo root (`hal/sub_screen.cpp`, `res/walk_window.rc`, `tools/battery.py`, `third_party/stb/stb_image.h`). `git rev-list --count main` under 200 means the playable history was not imported. Do not push that result.
- `decomp-main` has the smoke suite at repo root (`slice_gate1.txt`, `tools/hostgen.py`, `hal/shims.cpp`). It does not need `hal/sub_screen.cpp`.
- Every decomp `port/*` branch, every `port-*` branch, `ci/port-build-proto`, and tag `v0.3.3` that existed at the start also exist on the new repo.
- Commit subjects survive. `port: gate 1 - decompiled code runs natively on PC` is reachable from `decomp-main`. A current integration-branch subject is reachable from `main`.
- The only push to `tangosdev/sm64ds-decomp` is the cleanup branch. Decomp `main` is not force-pushed. No decomp `port/*` branch is deleted.
- The first push to `sm64ds-port` is a normal push to an empty repository.
- The port repo has no ROM, `extracted/` dump, `mwccarm`, `license.dat`, claims key, or `ghp_` / `github_pat_` / `AKIA` token.
- `SPLIT.md`, `DECOMP-PINS.tsv`, `LICENSE`, `.gitignore`, and `tools/check_nest.py` are on both `main` and `decomp-main`.
- Smoke CI on `decomp-main` configures and links `smoke`, `smoke_gx`, `smoke_heap`, `smoke_roots`, and `smoke_fs` against a sparse checkout of decomp `main`.
- Playable CI on `main` checks out the recorded decomp pin with `port/` excluded, nests this repo at `port/`, and `python tools/check_nest.py` exits 0.
- A pull request is open against decomp `main` that deletes `port/`, deletes `.github/workflows/port-build.yml`, gitignores `/port/`, makes `tools/port_refcheck.py` skip when `port/` is absent, and retargets living docs. It does not rewrite history. Do not merge it.

## Invariants

### Nest, do not retarget the build

The directory that contains `port/` is the decomp root. Keep that.

- Slice lines are decomp-root-relative (`src/DotVec3.c`, `port/unmatched/ModelComponents_Render.cpp`). `ov*_syms.txt` is a symbol table. CMake appends `${CMAKE_CURRENT_SOURCE_DIR}/../${line}` and adds `${CMAKE_CURRENT_SOURCE_DIR}/../include`. On the playable file, `PORT_HOSTGEN_TU_ROOT` is `${CMAKE_CURRENT_SOURCE_DIR}/..` (line 1075 of `port/link100` on 2026-09-23) and `facegen.py` is passed `--root` of that same parent. Re-read the live file; do not edit it.
- `port/tools/hostgen.py` defaults `--decomp` to `Path(__file__).resolve().parents[2]`. CMake on `main` invokes it without `--decomp`. It imports `<decomp>/tools/srcpath.py`.
- `romdata.py` and `ntr_manifest.py` use `parents[2]` and read `<decomp>/extracted/arm9_dec.bin` and `<decomp>/build/assets/files.tsv`. Those are gitignored outputs of `tools/unpack.py` and `tools/asset_catalog.py`. They stay out of the port repo.
- `host_frontier.py`: `REPO = parents[2]`, `SRC = REPO/src`, `INCLUDE = REPO/include`, `PORT = REPO/port`.
- `build-port.cmd` requires `%~dp0..\extracted\arm9_dec.bin`, `%~dp0..\build\assets\files.tsv`, and `handles.tsv`, then configures `-S %~dp0.` and `-B %~dp0..\build\port`.
- `PORT_REPO_ROOT` is the parent of `port/`. `hal/fs.cpp` uses it as the asset root (`SM64DS_ASSET_ROOT` overrides). `host_abi.h` rejects a 64-bit host. The MSVC build is 32-bit (`vcvars32`).

**Strip the `port/` prefix, then nest the port repo at `<sm64ds-decomp>/port/`.** `git filter-repo --subdirectory-filter port` does the strip. Do not also pass `--path-rename`. After the strip, nesting recreates `../src`, `../include`, `../tools/srcpath.py`, `../extracted`, `../build/assets`, and the decomp-root-relative slice text `port/unmatched/...`.

Do not rewrite `CMakeLists.txt`, `hostgen.py`, `romdata.py`, `build-port.cmd`, or the slice manifests. A sibling clone does not match `CMAKE_CURRENT_SOURCE_DIR/..` or `parents[2]`. A submodule either puts `src/` at `decomp/src` or puts a gitlink back in the matching repo, and one pin cannot serve both decomp `main` and the playable line. Reject both.

Exclude the decomp's tracked `port/` before nesting, or the trees collide. This works on decomp `main` and on playable pins (those commits still track `port/`):

```sh
git sparse-checkout init --no-cone
git sparse-checkout set '/*' '!/port/'
```

That leaves `src/`, `include/`, and `tools/srcpath.py`, and removes `port/`.

Playable `main` builds only against its recorded decomp pin. Smoke `decomp-main` builds against decomp `main`. Nesting the playable tree into a decomp `main` worktree fails `port_refcheck` and the slice paths. That failure is expected. Use two decomp worktrees. Do not edit slices to make the playable tree build against decomp `main`.

### What moves, what stays

**Moves, via a `port/`-only history filter.** The whole `port/` tree on every selected ref, prefix stripped (`port/hal/fs.cpp` becomes `hal/fs.cpp`): `CMakeLists.txt`, `README.md`, `build-port.cmd`, `build-port-trace.cmd`, `host_abi.h`, `hal/`, `ntr/`, `tests/`, `tools/`, `unmatched/`, `docs/`, `res/`, `third_party/`, every `slice*.txt`, every `ov*_syms.txt`, and the rest of the playable top-level text files.

**Copied by hand** (they are not under `port/`, so `--subdirectory-filter port` drops them):

- `.github/workflows/port-build.yml` from decomp `main`. Delete it on the decomp. Recreate it on port `decomp-main` only, using the workflow in this prompt.
- `notes/port-selftest-bmp-gate.md` from decomp `main`. Copy to `docs/port-selftest-bmp-gate.md` on port `main` (the playable tree has `tools/battery.py`, `hal/host_settings.cpp`, and `ov009_syms.txt`). In the copy, rewrite `port/tools/` → `tools/`, `port/hal/` → `hal/`, `port/ov` → `ov`. Then delete the note from the decomp.

**Stays in the decomp:**

- All of `src/` and `include/`, including every `#ifndef SM64DS_PLATFORM_PC` guard. Those guards keep mwccarm `sizeof` asserts while the host build defines `SM64DS_PLATFORM_PC`. Do not delete, move, or edit them in the cleanup PR.
- `tools/srcpath.py`, `tools/unpack.py`, `tools/asset_catalog.py`, `config/**`, the matching workflows, `tools/hooks/pre-push` (comment and echo only), `tools/port_refcheck.py` and `tools/test_port_refcheck.py` (the checker stays and skips when `port/` is absent), `tools/cpp_tu_compat.py` (it imports `port_refcheck`), `tools/validate_merge.py`'s optional `--port-refcheck-report` (an absent report is already neutral), and `config/port_linkage.json` (update `_comment` only; do not change `linkedTus`, `matchedTus`, `branch`, `commit`, or `measuredAt`).
- Decomp `port/*`, `port-*`, and `ci/port-build-proto` branches, and tag `v0.3.3`. They remain the combined snapshot (decomp `src/` at that commit plus the old in-tree `port/`). The playable pin builds against that snapshot.

**Does not move:**

- `src/` edits that exist only on a `port/*` branch. The filter drops them from the port repo. They stay on that decomp branch. Building that port branch means checking out that same decomp SHA. Do not filter `src/` or `include/` into the port repo.
- `mods/Player_ScaleByCharFactor.c` (on the `v0.3.3` tag, not under `port/`).
- Matching branches (`cpp/*`, `agents/*`, `fix/*`, and the rest). A name that merely contains `port` is not a port branch (`docs/profile-lifecycle-crosswalk`).

### Constraints

- Rewrite history only in a fresh mirror clone, never in a matching worktree. If `git filter-repo` demands `--force`, halt. That refusal means this is the wrong repository.
- No `--force`, `--force-with-lease`, or `--mirror` push. An empty repo accepts a normal push.
- Before every push, the destination URL contains `tangosdev/sm64ds-port` and does not contain `sm64ds-decomp`.
- Create `sm64ds-port` only as `tangosdev`. If the name is taken by someone else, or the repo exists and is not empty, halt and report. Do not delete a repository.
- No ROM, `extracted/` tree, `mwccarm`, `license.dat`, claims key, or token in any commit. `res/walk_window.ico` on the playable tree is a plain window glyph (dark rounded square, lighter pane). Keep it. A character sprite, level texture, `.nds`, `.bmd`, or `romdata.bin` does not get pushed. Strip a bad blob with a second `git filter-repo` before the first push. After the first push, do not rewrite. If a bad blob is already pushed, halt and report. Do not force-push a fix.
- Do not merge the playable tree and the smoke suite.
- Do not edit the playable `CMakeLists.txt` to change path resolution.
- Do not delete decomp `port/*` branches.
- The cleanup PR does not touch decomp `src/` or `include/`.

### Halt and report

Halt and report to the operator only for an irreversible case:

- `gh auth status` is not `tangosdev`.
- `tangosdev/sm64ds-port` exists and is not empty, or the name is taken by another owner.
- A secret or forbidden blob is already in a commit that has been pushed. Do not print the secret.
- A push would go to `sm64ds-decomp`, or `git filter-repo` is about to run outside the fresh mirror.
- The remote rejects the first push because it is not empty.

A local filter that fails a sentinel check is redone from a new mirror before any push. That is a failed step, not a question for the operator.

## Procedure

### 0. Account, empty target, rewriter

```sh
gh auth status
gh repo view tangosdev/sm64ds-port --json name,isEmpty,url
```

The active account is `tangosdev`. A 404 means the name is free for this token. The token needs `repo` scope so a private existing repo is not invisible. If the repo exists and `isEmpty` is false, halt and report. If it exists and is empty, push to it later. If it does not exist, create it only after the local filter is scanned:

```sh
gh repo create tangosdev/sm64ds-port --public --disable-wiki \
  --description "Native PC port of Super Mario 64 DS. Byte-matching source stays in tangosdev/sm64ds-decomp."
```

Do not pass `--clone`. Do not let `gh` add a README or a license. The first commit is the filtered history.

```sh
python3 -m pip install --user git-filter-repo
git filter-repo -h | head -n 80
```

`--subdirectory-filter` and `--refs` must be in that help. Do not fall back to `git filter-branch`.

### 1. Fresh mirror

```sh
mkdir -p "$HOME/sm64ds-port-split"
cd "$HOME/sm64ds-port-split"
git clone --mirror https://github.com/tangosdev/sm64ds-decomp.git decomp-mirror.git
cd decomp-mirror.git
git remote get-url origin
git rev-parse --is-bare-repository
```

Origin is `https://github.com/tangosdev/sm64ds-decomp.git` (or the SSH form of that repo). `--is-bare-repository` prints `true`.

Keep a ref only when it is `main`, `port/<anything>`, `port-<anything>`, `ci/port-build-proto`, or tag `v0.3.3`. On 2026-09-23 that was 56 `port/*` branches, 10 `port-*` branches (`port-0.2.7-hotfix`, `port-fix-ledgehang`, `port-fix-momentum`, `port-fix-walljump`, `port-gate-10`, `port-infra-consolidated`, `port-mount-noseat-cluster`, `port-particles`, `port-portable-kit`, `port-stream-b`), plus `main`, `ci/port-build-proto`, and `v0.3.3`. Re-read the live list.

```sh
git for-each-ref --format='%(objectname) %(refname)' \
  refs/heads/main \
  'refs/heads/port/*' \
  'refs/heads/port-*' \
  refs/heads/ci/port-build-proto \
  refs/tags/v0.3.3 | tee "$HOME/sm64ds-port-split/pins-before.txt"
```

The integration ref must be in that file. Read it from `git show main:config/port_linkage.json` (`branch`, expected `port/link100`). If that branch does not exist, halt and report the missing ref. Also require `refs/heads/port/release-0.4.0` and `refs/tags/v0.3.3` when they still exist upstream. A live set far below the 2026-09-23 inventory means the mirror is incomplete; re-clone before filtering.

Record the tips separately so they do not get appended to the `SHA ref` list:

```sh
git rev-parse refs/heads/main refs/heads/port/link100 | tee "$HOME/sm64ds-port-split/tip-shas.txt"
```

Use the live integration ref in that command if it is not `port/link100`.

### 2. Filter

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

Do not pass `--path-rename` or `--invert-paths`. `git filter-repo` removes `origin`. `git remote -v` must print nothing. If `origin` still points at `sm64ds-decomp`, `git remote remove origin` only while you are still inside this mirror and have not pushed.

```sh
{
  echo -e "ref\tdecomp_sha\tport_sha"
  while read -r old ref; do
    new=$(git rev-parse "$ref")
    printf '%s\t%s\t%s\n' "$ref" "$old" "$new"
  done < "$HOME/sm64ds-port-split/pins-before.txt"
} | tee "$HOME/sm64ds-port-split/DECOMP-PINS.tsv"
```

`pins-before.txt` is only `for-each-ref` output: each line is `SHA ref`. Do not append anything else. Every `decomp_sha` is the pre-filter SHA. Every `port_sha` is the post-filter SHA. They differ.

### 3. Branch names

```sh
git branch -m main decomp-main
git branch main refs/heads/port/link100
git symbolic-ref HEAD refs/heads/main
```

If the integration branch is not `port/link100`, use the live name. After this, `main` and `port/link100` point at the same filtered commit, before the docs commits. `decomp-main` is filtered decomp `main`. Do not delete `port/link100`.

```sh
git cat-file -e main:hal/sub_screen.cpp
git cat-file -e main:res/walk_window.rc
git cat-file -e main:tools/battery.py
git cat-file -e decomp-main:slice_gate1.txt
git cat-file -e decomp-main:tools/hostgen.py
git cat-file -e decomp-main:hal/shims.cpp
git merge-base --is-ancestor \
  "$(git rev-list --max-count=1 --grep='port: gate 1' decomp-main)" decomp-main
git rev-list --count main
```

Any missing path, or a `main` count under 200, means the filter or the rename is wrong. Delete the mirror and redo from step 1. Do not push.

### 4. Docs commits

A bare repo treats `HEAD` as a checkout. Point it at a ref you will not edit, or `git worktree add` of `main` / `decomp-main` fails with "already checked out".

```sh
git symbolic-ref HEAD refs/heads/ci/port-build-proto
git worktree add "$HOME/sm64ds-port-split/wt-main" main
cd "$HOME/sm64ds-port-split/wt-main"
```

`LICENSE` lived at the decomp root, so the filter dropped it. Take it from GitHub:

```sh
gh api repos/tangosdev/sm64ds-decomp/contents/LICENSE?ref=main --jq .content | base64 -d > LICENSE
```

The file starts with `MIT License` and contains `Copyright (c) 2026 Tango`. Do not edit it.

`.gitignore`, exactly:

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

`tools/check_nest.py`, exactly:

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

Copy `$HOME/sm64ds-port-split/DECOMP-PINS.tsv` to the worktree root.

Write `SPLIT.md` with these facts, in order:

1. This repository is the old `port/` directory of https://github.com/tangosdev/sm64ds-decomp with the `port/` prefix removed. Byte-matching source stays in the decomp repo.
2. Clone the decomp, exclude its tracked `port/`, then clone this repo into that checkout at a directory named `port`:

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

3. Branch `main` of this repo (and the integration branch) builds against the `decomp_sha` for that integration ref in `DECOMP-PINS.tsv`. Branch `decomp-main` builds against decomp `main`. Do not point the playable branch at decomp `main`, or the smoke branch at the playable pin.
4. Use a second decomp worktree for matching work. Do not nest the playable tree in the decomp `main` worktree you push matching commits from. Decomp `tools/port_refcheck.py` still runs when `port/` exists, and the playable slices do not match decomp `main`'s `src/`.
5. Assets stay in the decomp checkout. From the decomp root, with a cartridge dump you own: `python tools/unpack.py path/to/your-own-sm64ds.nds` then `python tools/asset_catalog.py generate path/to/your-own-sm64ds.nds`. Then, on Windows with the 32-bit MSVC toolset, run `port\build-port.cmd`. The ROM, `extracted/`, and `build/assets/` are gitignored.
6. `hostgen.py`, `romdata.py`, and `build-port.cmd` find the decomp as the parent of this repo. That holds only while this repo is checked out at `<decomp>/port`. `--decomp` is for a one-off `hostgen.py` invocation from another directory.
7. MIT, same `LICENSE` as the decomp, covering original code only. No ROM and no Nintendo assets. `third_party/stb/` is the public-domain stb headers. `res/walk_window.ico` is a plain window icon.
8. After the split, a `src/` rename can strand a slice path. Fix that in this repo: `decomp-main` for the smoke suite, the playable branch when the pin moves. Decomp `main` no longer carries `port/`.
9. SHAs in this repo are not decomp SHAs. `DECOMP-PINS.tsv` maps them. `git filter-repo` rewrote every published commit.

```sh
git add LICENSE .gitignore SPLIT.md DECOMP-PINS.tsv tools/check_nest.py
git commit -m "Record how this repo nests back into sm64ds-decomp"
```

On this same branch, copy the playable selftest note and rewrite its path prefixes (`port/tools/` → `tools/`, `port/hal/` → `hal/`, `port/ov` → `ov`):

```sh
gh api repos/tangosdev/sm64ds-decomp/contents/notes/port-selftest-bmp-gate.md?ref=main --jq .content \
  | base64 -d > docs/port-selftest-bmp-gate.md
git add docs/port-selftest-bmp-gate.md
git commit -m "Move the walk_window selftest note into the port repo"
```

Add `.github/workflows/nest-check.yml` on `main` only. `DECOMP_REF` is the pin file, not a SHA copied from this prompt:

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

If the integration branch is not `refs/heads/port/link100`, change that awk string to the live ref.

```sh
git add .github/workflows/nest-check.yml
git commit -m "Check playable slice paths against the recorded decomp pin"
```

Cherry-pick only the license / gitignore / SPLIT / pins / `check_nest` commit onto `decomp-main`. Do not cherry-pick the selftest note or `nest-check.yml`.

```sh
DOCS=$(git log --format='%H' --grep='^Record how this repo nests back into sm64ds-decomp$' -n 1 main)
test -n "$DOCS"
git --git-dir="$HOME/sm64ds-port-split/decomp-mirror.git" symbolic-ref HEAD refs/heads/ci/port-build-proto
git worktree add "$HOME/sm64ds-port-split/wt-smoke" decomp-main
git -C "$HOME/sm64ds-port-split/wt-smoke" cherry-pick "$DOCS"
```

If the cherry-pick conflicts, halt and report. Do not resolve it by taking the playable `CMakeLists.txt`.

On `wt-smoke`, add `.github/workflows/port-build.yml` exactly as follows and commit it with subject `Run the smoke port build against decomp main`:

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

Until the cleanup PR merges, decomp `main` still has `tools/port_refcheck.py` and a tracked `port/`. Sparse-checkout removes that tree; the nest puts the filtered smoke suite back. `port_refcheck.py` on current decomp `main` understands the nine `slice_gate` manifests. Do not point this workflow at the playable pin.

Those five targets are the ones the old workflow links without a ROM. Do not add `smoke_model` or any target that runs `romdata.py`. `BUILD_TESTING` stays off. No cartridge is fetched.

If `git sparse-checkout` from `cmd` mis-parses the quotes, run that step with `shell: bash` and leave only the `vcvars32` / `cmake` lines in `cmd`.

### 5. Scan, then push

From `wt-main`, fetch the mirror refs (the worktrees share the mirror object store) and scan every branch:

```sh
git fetch "$HOME/sm64ds-port-split/decomp-mirror.git" \
  'refs/heads/*:refs/heads/*' 'refs/tags/*:refs/tags/*'

git ls-tree -r --name-only --all \
  | grep -Ei '\.(nds|sav|ml[0-9]+|bmd|nsbmd|arc)$|(^|/)(mwccarm|license\.dat|claims_key|romdata\.bin)(/|$)' \
  || true

git rev-list --all --objects \
  | git cat-file --batch-check='%(objecttype) %(objectname) %(objectsize) %(rest)' \
  | awk '$1=="blob" && $3+0 > 1000000 {print}'

git log --all -S 'ghp_' --oneline
git log --all -S 'github_pat_' --oneline
git log --all -S 'AKIA' --oneline
```

The playable `CMakeLists.txt` (about 1.3 MB) is an allowed blob over 1 MB. A `.bin`, `.nds`, texture dump, or archive is not. The three `-S` logs are empty. If a secret is in history, halt before the push and report that a secret was found. Do not print it.

Create the empty repo now if step 0 did not. Confirm `git rev-parse main` in the mirror is the nest-check commit, and `git rev-parse decomp-main` is the port-build commit, not the raw filtered tips. Push with an explicit URL. Do not add a remote that points at the decomp.

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

If a push is rejected because the remote is not empty, halt and report. Do not force-push.

```sh
gh repo edit tangosdev/sm64ds-port --default-branch main
```

### 6. Verify

```sh
gh repo view tangosdev/sm64ds-port --json defaultBranchRef,url --jq '{branch:.defaultBranchRef.name,url:.url}'
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

The `port/` head count matches the pre-filter `port/*` count. `v0.3.3` exists. The `main` count is the step 3 count plus the docs commits. Default branch is `main`.

Smoke nest against decomp `main` (no MSVC required):

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

`check_nest.py` prints `nest ok`. `port_refcheck.py` prints `all references resolve`.

Repeat the nest for the playable pin: check out the `decomp_sha` of the integration ref from `DECOMP-PINS.tsv`, sparse-exclude `port/`, clone the port repo, check out `main`, run `python port/tools/check_nest.py`. That passes. Do not run playable `port_refcheck` from a decomp `main` checkout.

The MSVC link of the five ROM-free targets is the `port-build` workflow on `decomp-main`. Run it locally when a Windows VM with the x86 toolset is available. Otherwise the `windows-latest` job is the proof. Do not fetch a ROM.

### 7. Decomp cleanup PR

Open this only after the port push and both nest checks have succeeded.

```sh
git clone https://github.com/tangosdev/sm64ds-decomp.git "$HOME/sm64ds-port-split/decomp-cleanup"
cd "$HOME/sm64ds-port-split/decomp-cleanup"
git checkout -b split/remove-port-from-decomp origin/main
git remote get-url origin
```

Origin is `tangosdev/sm64ds-decomp`.

```sh
git rm -r port
git rm .github/workflows/port-build.yml
git rm notes/port-selftest-bmp-gate.md
```

Append to `.gitignore`:

```
# PC port. Separate repository: https://github.com/tangosdev/sm64ds-port
# Clone it into this directory (the directory name must stay `port`).
/port/
```

In `tools/port_refcheck.py` `main`, before `report = build_report()`:

```python
    if not PORT.is_dir():
        print("port-refcheck: skipped (port/ not checked out; "
              "the PC port is https://github.com/tangosdev/sm64ds-port)")
        return 0
```

Do not change `check_hal_links` or the unit-test fixtures. Those tests call the check functions directly and create their own `port/` directory. `python -m unittest tools.test_port_refcheck tools.test_cpp_tu_compat` still passes. With `port/` deleted, `python tools/port_refcheck.py` exits 0 and prints `skipped`.

Update the comment block at the top of `tools/hooks/pre-push` and the echo on the port check: the check skips when `port/` is not checked out, and a playable nest does not belong in a decomp `main` worktree.

Update `config/port_linkage.json` `_comment` only. The refresh command is: check out the port repo at `port/` inside the decomp worktree that matches the stamp's branch, build that tree, then from the decomp root run `python port/tools/linkage.py .`.

Retarget living docs off relative `port/` paths. Point them at `https://github.com/tangosdev/sm64ds-port`. Where a file path is needed: `https://github.com/tangosdev/sm64ds-port/blob/main/README.md` (playable) and `https://github.com/tangosdev/sm64ds-port/blob/decomp-main/README.md` (smoke). State that:

- `src/` platform guards stay in the decomp.
- Slice manifests and `hal/` bridges live in `sm64ds-port`.
- A rename that strands a smoke slice is fixed on `sm64ds-port` branch `decomp-main`.
- `python tools/port_refcheck.py` runs when a port checkout is nested at `port/`, and skips when it is not.
- Do not nest the playable branch into the worktree used for matching pushes.

Edit these files (they exist on decomp `main`):

- `README.md` (the LINKED paragraph and the `port/` link; keep `https://tangos.dev/downloads`)
- `AGENTS.md` (the `port/` references section)
- `MERGE.md` (the `port/` reference-break paragraph)
- `notes/pr-validation.md`
- `notes/runbook-reference-repair.md`
- `notes/real-cpp-migration-runbook.md`
- `notes/agents/references/writer-v1.md`
- `notes/agents/references/pipeline-v1.md`
- `notes/source-review-observations.md` (relative links to `../port/README.md`, `hostgen.py`, and `host_frontier.py`)
- `tools/tiers.py` (the comment that LINKED reads `port/tools/linkage.py`)
- `tools/tubuild.py` (the print that tells the user to edit `port/CMakeLists.txt`)

Leave historical handoffs, `notes/collision-system.md`, `notes/ctor-migration.md`, `CLAIMS.md`, and `notes/system-provenance.md` alone unless `python tools/check_dead_references.py` reports a new failure in them. For a living instruction, retarget the URL. For a historical `git show` of an old decomp branch (`origin/port-mount-noseat-cluster:port/unmatched/...`), keep the command: those branches still exist. Bank a dead path only if the checker requires it. Do not run `--update` or `--update-code` as a blanket refresh. A healed baseline row (a banked `port/` path whose citing file was deleted) is not a failure. Mention it in the PR. Do not bank a new dead link in place of a doc that should have been retargeted.

`tools/cpp_tu_compat.py` keeps importing `port_refcheck`.

The cleanup diff contains no `src/` or `include/` file changes. If a comment in `src/` is the only way to clear a new dead-reference failure, reword that comment and no other line, and say so in the PR.

```sh
python -m unittest tools.test_port_refcheck tools.test_cpp_tu_compat -v
python tools/port_refcheck.py
python tools/check_dead_references.py
gh api repos/tangosdev/sm64ds-decomp/branches/main/protection
gh api repos/tangosdev/sm64ds-decomp/rulesets
```

`port_refcheck.py` skips. `check_dead_references.py` exits 0.

Deleting `.github/workflows/port-build.yml` makes required checks named for that workflow (`refs` and `build` under workflow `port-build`) never report, which blocks every later decomp PR. Remove only those required contexts. Do not disable `validate`, source review, or any other required check. If the API refuses the edit, report the check names. Do not merge past a stuck required check.

Commit, `git push -u origin split/remove-port-from-decomp`, and open a PR into `main`. Title:

```
Move the PC port tree to tangosdev/sm64ds-port
```

Body, with the live SHAs and nest-check results filled in:

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
- Do not delete the decomp's port/* branches until each one has a pin in the port repo and the operator confirms the combined branch can go.
```

Do not merge the PR.

## Mixed commits

On the decomp `main` used for this research (`9c6104f` on the lunavyqo fork, behind tangosdev `e04ccbc`), 73 commits touch `port/`. 20 touch only `port/` (the 2026-08-02 gates from `9651d82a5172393584227a2190e34d3fde25a09b`, subject `port: gate 1`, PR #1001, through gate 9, plus a few later port-only follow-ups). The other 53 also change `src/`, `include/`, `config/`, sometimes thousands of files. The port hunk is often one slice line inside a rename or a TU promotion (`dActor_c` rename PR #1577: 6 files under `port/`, 2194 elsewhere).

`--subdirectory-filter port` keeps those commits and drops the non-`port/` files. A rename subject whose only remaining diff is a slice line is the correct result. Those slice updates are why the smoke suite still resolves. Do not drop mixed commits and do not squash them into one initial import.

Recompute on the mirror if the cleanup PR body should carry live counts. Do not block the split on matching 53 and 73.

```sh
git rev-list main -- port | wc -l
```

Count, per commit, `git diff-tree --name-only -r` paths that do not start with `port/`.

Filtering a playable branch keeps its `port/` files and drops `src/` edits that exist only on that branch. The decomp pin still has those `src/` edits. A filtered playable commit does not build against decomp `main`. Do not copy `src/` into the port repo to make it do so.

## Rollback

- Before the first push: delete `$HOME/sm64ds-port-split`. The decomp is untouched.
- After the port repo is pushed, before the cleanup PR merges: leave the port repo. Close the cleanup PR without merging. Decomp `main` still has `port/`.
- After the cleanup PR merges and the result is wrong: open a normal revert PR of that merge. Do not force-push decomp `main`.
- Do not delete `tangosdev/sm64ds-port` as a rollback. Halt and report.

## Report

- Port repo URL, default branch, `git rev-list --count` for `main` and `decomp-main`.
- `decomp_sha` for the integration ref and for decomp `main`.
- How many `port/*`, `port-*`, and other refs were pushed, and whether `v0.3.3` was pushed.
- Nest-check result for both branches.
- Whether the five ROM-free targets linked, and if not, the CI URL.
- Blob-scan hits that were kept or stripped, by path. No secret values.
- Cleanup PR URL, and `git diff --stat` showing `src/` and `include/` absent.
- Branch-protection change, or the required-check names still to remove.
- Any live SHA that disagreed with an example in this prompt.
