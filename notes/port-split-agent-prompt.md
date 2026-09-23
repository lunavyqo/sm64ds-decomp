# SM64DS: split PC port into its own repo

Plan and requirements for a later executor. This document does not authorize running the split.

## Goal

Move the PC port out of `tangosdev/sm64ds-decomp` into a new public repo `tangosdev/sm64ds-port`, with history preserved, then open a normal cleanup PR on the decomp that drops the in-tree `port/` from **decomp `main` only**.

Operator and GitHub identity for every create and push: `tangosdev`. Do not push the split to forks, including `lunavyqo/sm64ds-decomp`.

Do not redesign build wiring. Do not invent a different history shape than the constraints below.

Live GitHub state wins over every SHA in this note. Dated figures are sanity checks from 2026-09-23.

## Critical facts

### Two different trees, both named `port/`

They are not interchangeable. Publish both. Do not merge them into one history.

| Tree | Where it lives today | What it is |
|------|----------------------|------------|
| **Smoke suite** | decomp `main` under `port/` | `CMakeLists.txt`, `README.md`, `build-port.cmd`, `host_abi.h`, `docs/`, `hal/` (~20 files), `ntr/`, `tests/`, five tools (`hostgen.py`, `host_frontier.py`, `linkage.py`, `ntr_manifest.py`, `romdata.py`), two `unmatched/` `.cpp` files, and only `slice_gate1`, `2`, `3a`, `3b`, `4b`, `6`, `7`, `8`, `9`. This is what `.github/workflows/port-build.yml` builds. Path-compatible with current matching `src/`. Not the downloadable game. |
| **Playable port** | `port/*` and `port-*` | The integration branch is `config/port_linkage.json` field `branch` (then `port/link100`, tip `c0a3e1da770bad822c040ceacf4850f3b7aeac1e`, thousands of commits off main). Hundreds of `hal/` files, `CMakeLists.txt` ~1.3 MB / ~22k lines, `res/`, `third_party/stb/`, `tools/battery.py`, `tools/romblob.py`, `ov*_syms.txt`, hundreds of `slice_*.txt`. Annotated tag `v0.3.3` (message starts `0.3.3: generic controllers`) sits on this line. https://tangos.dev/downloads is this line. `port/release-0.4.0` was `6230b2996636e3fd0e03cb178161c94cb4b73cc9`. |

**New-repo shape**

- Default branch `main` = filtered **playable** integration branch (whatever `branch` names live).
- Branch `decomp-main` = filtered **smoke** suite from decomp `main`.
- Also publish every live `port/*`, every `port-*`, `ci/port-build-proto`, and tag `v0.3.3`.
- A name that merely contains `port` is not a port ref (`docs/profile-lifecycle-crosswalk`).

On that date the inventory was 56 `port/*` branches and 10 `port-*` branches, plus `main`, `ci/port-build-proto`, and `v0.3.3`. Re-count live. A set far smaller than that is an incomplete mirror.

### Nest wiring (do not redesign)

The directory that contains `port/` is the decomp root.

- Slice lines are decomp-root-relative (`src/...`, `port/unmatched/...`). `ov*_syms.txt` is a symbol table, not a path list. CMake appends `${CMAKE_CURRENT_SOURCE_DIR}/../${line}` and adds `../include`. Playable CMake sets `PORT_HOSTGEN_TU_ROOT` to that same parent and passes it as `facegen.py --root`. Re-read the live file; do not edit it.
- `hostgen.py`, `romdata.py`, `ntr_manifest.py`, and `host_frontier.py` use `parents[2]` when the script lives at `<decomp>/port/tools/...`. CMake invokes `hostgen.py` without `--decomp`. It then imports `<decomp>/tools/srcpath.py`.
- `romdata.py` / `ntr_manifest.py` read `<decomp>/extracted/arm9_dec.bin` and `<decomp>/build/assets/files.tsv` (gitignored outputs of `tools/unpack.py` and `tools/asset_catalog.py`). `build-port.cmd` refuses to run without those, then configures the port dir and builds into `<decomp>/build/port`.
- `PORT_REPO_ROOT` is the parent of `port/`. `hal/fs.cpp` uses it as the asset root (`SM64DS_ASSET_ROOT` overrides). `host_abi.h` rejects a 64-bit host; the MSVC build is 32-bit.

**Chosen layout:** strip the `port/` prefix so the port repo root is today's `port/` contents, then nest a checkout at `<sm64ds-decomp>/port/`. That restores `../src`, `../include`, `../tools/srcpath.py`, `../extracted`, `../build/assets`, and decomp-root-relative `port/unmatched/...`.

Intended mechanism: a subdirectory history filter that drops the `port/` prefix. Not `git filter-branch`. Not a second path rename on top. Not a squash of mixed commits. Run it only in a fresh mirror, never in a matching worktree. If the rewriter demands `--force`, that clone is the wrong repository.

Reject for this split:

- A sibling clone (`../sm64ds-decomp`). It does not match `..` or `parents[2]` unless the ~22k-line playable CMake and the scripts are rewritten.
- A git submodule. It puts `src/` in the wrong place or puts a gitlink back in the matching repo, and one pin cannot serve both decomp `main` and the playable line.

Decomp checkouts used for nesting must exclude the decomp's own tracked `port/` (sparse-checkout is the known way) or the two trees collide. That applies to decomp `main` and to playable pins; those commits still track `port/`.

Playable `main` builds only against its recorded decomp pin (the pre-filter SHA of the integration branch). Smoke `decomp-main` builds against decomp `main`. Nesting playable into a decomp `main` worktree fails `port_refcheck` and the slice paths. That failure is expected. Use two decomp worktrees. Do not edit slices to paper over it.

## Plan (what, not how)

1. **Inventory** live refs, SHAs, and `config/port_linkage.json` on `tangosdev/sm64ds-decomp`. Dated examples are stale until re-read.
2. **Create** empty public `tangosdev/sm64ds-port`, or use it only if it already exists and is empty. No generated README or license commit. Confirm the token can see private repos so an existing private repo is not mistaken for a free name.
3. **Extract** `port/` history for the selected refs, prefix stripped. Publish playable as `main`, smoke as `decomp-main`, plus the other port refs and `v0.3.3`.
4. **Hand-copy** port material that does not live under `port/` (list below). Add the nest record: split note, pin table, nest checker, decomp `LICENSE` (it lives at the decomp root, so a `port/`-only filter drops it), and a gitignore for ROM and build output.
5. **Scan** history for ROMs, `extracted/`, `mwccarm`, `license.dat`, claims keys, tokens, and Nintendo asset dumps. Strip before the first push only.
6. **Push** with a normal push to the empty port repo. Destination is `tangosdev/sm64ds-port` and is not the decomp. Set default branch `main`.
7. **Wire CI** on the port repo. Smoke on `decomp-main` against a sparse decomp `main`. Playable on `main` against the recorded pin, with the decomp's `port/` excluded and this repo nested at `port/`.
8. **Cleanup PR** on `tangosdev/sm64ds-decomp` `main` only, after the port push and both nest checks succeed. Normal branch push. No history rewrite. Do not merge it in this task.
9. **Branch protection:** if `port-build` jobs are required on decomp `main`, removing that workflow stalls every later decomp PR until those contexts are cleared. Remove only those contexts.

The nest record (license, gitignore, split note, pin table, nest checker) is on both `main` and `decomp-main`. The playable selftest note and the playable nest workflow stay on `main` only. The smoke workflow stays on `decomp-main` only. Do not take the playable `CMakeLists.txt` onto the smoke branch.

Pin table columns: ref, pre-filter decomp SHA, post-filter port SHA. Those two SHAs differ. Do not append extra lines to the ref inventory the table is built from.

The split note states: this repo is the old `port/` with the prefix removed; nest it at a directory named `port` inside a decomp checkout whose tracked `port/` is excluded; which decomp pin each branch builds against; matching work uses a second worktree; assets stay in the decomp checkout; parent-directory discovery is correct only at `<decomp>/port`; MIT, no ROM; later slice fixes land in this repo; port SHAs are not decomp SHAs.

Nest check: this repo's parent directory is named `port`; the decomp has `src/`, `include/`, and `tools/srcpath.py`; every `slice*.txt` path (comments and a leading BOM skipped) exists under the decomp. Do not treat `ov*_syms.txt` as a path list.

Smoke CI links only `smoke`, `smoke_gx`, `smoke_heap`, `smoke_roots`, and `smoke_fs`, with tests off, and fetches no cartridge. `port_refcheck` on current decomp `main` understands the nine gate manifests; that is the checker for `decomp-main`. Do not point smoke CI at the playable pin.

## What moves / stays / hand-copies

### Moves (via the `port/` history filter)

The whole `port/` tree on every selected ref, prefix stripped (`port/hal/fs.cpp` becomes `hal/fs.cpp`): CMake, README, `build-port.cmd`, `build-port-trace.cmd`, `host_abi.h`, `hal/`, `ntr/`, `tests/`, `tools/`, `unmatched/`, `docs/`, `res/`, `third_party/`, every `slice*.txt`, every `ov*_syms.txt`, and the other playable top-level text files when present.

### Hand-copy (a `port/`-only filter will not see these)

- `.github/workflows/port-build.yml` from decomp `main`. Delete it on the decomp. Recreate the smoke workflow on port `decomp-main` only.
- `notes/port-selftest-bmp-gate.md` from decomp `main`. Copy to `docs/port-selftest-bmp-gate.md` on port `main` (that tree has `tools/battery.py`, `hal/host_settings.cpp`, and `ov009_syms.txt`). Rewrite `port/tools/` → `tools/`, `port/hal/` → `hal/`, `port/ov` → `ov`. Delete the note from the decomp in the cleanup PR.

### Stays in the decomp

- All of `src/` and `include/`, including every `#ifndef SM64DS_PLATFORM_PC` guard. The cleanup PR does not edit them.
- `tools/srcpath.py`, `tools/unpack.py`, `tools/asset_catalog.py`, `config/**`, and the matching workflows.
- `tools/port_refcheck.py` and `tools/test_port_refcheck.py`. The checker stays. `main` returns success when `port/` is absent, so pre-push still works. Do not change `check_hal_links` or the tests' fixtures; those tests call the checks directly and create their own `port/`.
- `tools/cpp_tu_compat.py` keeps importing `port_refcheck`. `tools/validate_merge.py`'s optional `--port-refcheck-report` stays; an absent report is already neutral.
- `config/port_linkage.json` numbers (`linkedTus`, `matchedTus`, `branch`, `commit`, `measuredAt`). Update `_comment` only, so it describes a later refresh from the decomp root through `port/tools/linkage.py` (playable map at `build/port/walk_window.map`). This split does not refresh the numbers.
- `tools/hooks/pre-push`: comment and echo only, so they say the check skips when `port/` is absent and that a playable nest does not belong in a matching worktree.
- Decomp `port/*`, `port-*`, and `ci/port-build-proto` branches, and tag `v0.3.3`. They remain the combined snapshot (decomp `src/` at that commit plus the old in-tree `port/`). The playable pin builds against that snapshot.
- `src/` edits that exist only on a `port/*` branch. A `port/`-only filter drops them from the port repo on purpose. Building that port branch means checking out that same decomp SHA.

### Does not move

- Matching branches (`cpp/*`, `agents/*`, `fix/*`, and the rest).
- `mods/Player_ScaleByCharFactor.c` on `v0.3.3` (not under `port/`). Re-check the tag for anything else outside `port/`.

### Cleanup PR contents

Branch `split/remove-port-from-decomp`, title `Move the PC port tree to tangosdev/sm64ds-port`. Delete tracked `port/`, `.github/workflows/port-build.yml`, and `notes/port-selftest-bmp-gate.md`. Gitignore `/port/` and point at the new repo. Retarget living docs to `https://github.com/tangosdev/sm64ds-port` (playable README on `main`, smoke README on `decomp-main`), and keep `https://tangos.dev/downloads`.

Living docs and comments: `README.md` LINKED paragraph, `AGENTS.md`, `MERGE.md`, `notes/pr-validation.md`, `notes/runbook-reference-repair.md`, `notes/real-cpp-migration-runbook.md`, `notes/agents/references/writer-v1.md`, `notes/agents/references/pipeline-v1.md`, `notes/source-review-observations.md`, the `port/tools/linkage.py` comment in `tools/tiers.py`, and the `port/CMakeLists.txt` print in `tools/tubuild.py`.

Leave historical handoffs, `notes/collision-system.md`, `notes/ctor-migration.md`, `CLAIMS.md`, and `notes/system-provenance.md` alone unless `tools/check_dead_references.py` reports a new failure. A historical `git show` of a decomp `port-*` branch stays; those branches still exist. Do not blanket-refresh the dead-reference baseline. Do not bank a new dead link in place of a doc that should have been retargeted. The cleanup diff contains no `src/` or `include/` changes.

## Non-negotiables

- Identity is `tangosdev` only, for the new repo and for the decomp cleanup branch.
- No force-push, no `--force-with-lease`, and no mirror push. No force-push of decomp `main`. No deletion of decomp `port/*` branches.
- The first push to the port repo is a normal push to an empty repository. If the remote is not empty, stop. Do not delete a repository to make the name free.
- No ROM, `extracted/` tree, `mwccarm`, `license.dat`, claims key, or `ghp_` / `github_pat_` / `AKIA` token in port history or at the tip. `res/walk_window.ico` on the playable line is a plain window glyph; keep it. The ~1.3 MB playable `CMakeLists.txt` is an allowed large blob. A character sprite, level texture, `.nds`, `.bmd`, or `romdata.bin` is not.
- If a forbidden blob is found before the first push, strip it then. After the first push, do not rewrite history. If a forbidden blob was already pushed, stop and report. Do not force-push the fix.
- Do not rewrite playable `CMakeLists.txt` path resolution.
- Do not merge the smoke tree and the playable tree.
- Do not edit matching `src/` or `include/` in the cleanup PR.
- Do not merge the cleanup PR in this task.
- Live refs and `config/port_linkage.json` win over this note.

## Success checklist

- [ ] `https://github.com/tangosdev/sm64ds-port` is public, owned by `tangosdev`, default branch `main`
- [ ] `main` is the playable tree at repo root (`hal/sub_screen.cpp`, `res/walk_window.rc`, `tools/battery.py`, `third_party/stb/stb_image.h`). A history count under 200 means the playable line was not imported
- [ ] `decomp-main` is the smoke suite at repo root (`slice_gate1.txt`, `tools/hostgen.py`, `hal/shims.cpp`)
- [ ] Gate-1 subject `port: gate 1 - decompiled code runs natively on PC` is reachable from `decomp-main`. A current integration-branch subject is reachable from `main`
- [ ] Every inventoried `port/*`, `port-*`, `ci/port-build-proto`, and `v0.3.3` exists on the new repo
- [ ] `SPLIT.md`, the pin table, `LICENSE`, `.gitignore`, and the nest checker are on both branches
- [ ] No forbidden blobs or secrets in port history
- [ ] Smoke nest in decomp `main` passes the nest check and `port_refcheck` (`all references resolve`)
- [ ] Playable nest in the recorded decomp pin passes the nest check. Playable `port_refcheck` is not run from decomp `main`
- [ ] Smoke CI links the five ROM-free targets; playable CI checks slices against the recorded pin
- [ ] Cleanup PR is open on decomp `main` with the deletions, skip, gitignore, and doc retargets above. Decomp `main` was not force-pushed. No `src/` or `include/` file is in that diff
- [ ] Decomp `port/*` branches still exist
- [ ] `port-build` required checks were removed, or were not required. Other required checks (`validate`, source review) are unchanged. If the protection API refuses, the check names are in the report and the cleanup PR is not merged

## Known footguns

- On the researched decomp `main` (`9c6104f` on the lunavyqo fork, behind tangosdev `e04ccbc`), 73 commits touch `port/`. About 20 are port-only (gates from `9651d82`, subject `port: gate 1`, PR #1001). About 53 also change `src/`, `include/`, or `config/` (`dActor_c` rename #1577: 6 files under `port/`, 2194 elsewhere). The filter keeps the port hunk. A rename subject whose only remaining diff is a slice line is correct. Do not drop or squash those commits. Recompute live counts; do not block on 53 and 73.
- A filtered playable commit does not build against decomp `main`. The decomp pin still has the `src/` edits the filter dropped. Do not copy `src/` into the port repo.
- Nesting the wrong decomp SHA fails refcheck. Two worktrees. Do not "fix" slices during the split.
- Removing `port-build.yml` while jobs `refs` and `build` are required checks blocks every decomp PR.
- `tangosdev/sm64ds-port` 404s until it exists. Confirm emptiness as `tangosdev` before the first push.
- Fork `main` can lag upstream. Inventory `tangosdev/sm64ds-decomp`, not a fork.
- A bare mirror treats `HEAD` as a checkout. Adding a worktree of `main` or `decomp-main` fails while `HEAD` points at that branch.
- Cherry-picking the shared nest commit onto `decomp-main` must not take the playable CMake. The selftest note and the playable nest workflow do not belong on `decomp-main`.
- `port_refcheck` unit tests build a fixture `port/`. A skip that changes `check_hal_links` breaks them. `cpp_tu_compat` still imports the module.
- Rollback: before the first push, discard the local mirror. After the push, leave the port repo and close an unmerged cleanup PR. After the cleanup PR merges, revert with a normal PR. Do not force-push decomp `main`. Do not delete `tangosdev/sm64ds-port` as a rollback.

## Out of scope

- Performing this split in the session that only edits this note.
- Merging the cleanup PR.
- Deleting decomp `port/*` branches, `port-*` branches, or tag `v0.3.3`.
- Rewriting matching `src/` or `include/`, including platform-guard edits.
- Moving a playable pin forward onto newer decomp commits.
- Fetching a cartridge or running ROM-backed smokes.
- Style or history cleanup unrelated to the split.
- Any account other than `tangosdev`.
