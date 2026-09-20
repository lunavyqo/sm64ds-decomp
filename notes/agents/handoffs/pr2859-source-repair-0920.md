# Handoff: pr2859-source-repair-0920

This document describes this commit. It is a WIP checkpoint for a queue resource
amendment, not independent acceptance or a merge candidate.

- Producer: codex-rnk-producer-0920 (Codex), stage `produce`.
- Input: `166dffa1d0767416bd386294a324f00ec267fcc9`; original source base:
  `4407c8471ed3299ea16d9cc899020e3232ac6ee9`.
- Branch: `review/pr2859-0920`; worktree: `C:/tmp/sm64ds-review2859-0920`.
- Protocol pin: `18af52b49b9fb5663d417bdfa86109b7b157aaa3`.
- PR: https://github.com/tangosdev/sm64ds-decomp/pull/2859.

## Changes and findings

RNK-01/02: production provenance is now truthful. Removed the deleted-file
comment references and the unsupported daNknk_c::Kill/vtable attribution.
RNK-03: all 16 non-destructor definitions have unique symbol markers; lifecycle
ownership remains the inline destructor instantiated by the factory.
RNK-04: the two word-sized coordinate fields now have 32-bit types with unchanged
offsets. The stored Player address has a pointer type; the total size remains 0x3e0.
RNK-05: the owned TU uses the existing Timer class and real member calls. Two
external Timer consumers still need reservation and repair before acceptance.
RNK-06: real Vector3 and SharedFilePtr replace fabricated local layouts. Native
C++ member-function-pointer dispatch replaces manual decoding. Initialization,
animation/collision updates, file loading/release, messages and star spawning now
use real members/interfaces wherever the tested form preserves bytes.

The 11 address-named helpers still have raw actor field views and several scalar
ABI calls. Reconstruction is partial. No claim is made that this is recovered
original source or that all possible cleaner forms have been exhausted. The
experiments record exact rejected alternatives for volatile particle coordinates,
the redundant conditional, the Render scale copy, fixed-point SetAnim and the
signed-short angle boundary. Compile failures are explicitly distinguished from
compiled nonmatches. Remaining broader work requires a concrete follow-up issue
and next owner before partial source acceptance; the coordinator owns publication.

## Proof and limits

[Committed experiments](../../experiments/pr2859-source-repair-0920.json) contain
replayable source/header patches, flags, sizes, function results and complete
emitted-object signatures. All 18 functions / 6108 bytes remain exact with clean
object isolation and relocation destinations. The weak Vector3 destructor moves
earlier; its body and sole-home duplicate policy remain unchanged. Every allocated
section body, definition and relocation agrees after identifying sections by owner.
Emitted data: 6 verified / 180 bytes, 5 partial / 44 bytes, zero differences or
unnamed symbols. The header has one consumer, this TU; all 26 commented offsets
check, with no unparsed fields.

Pristine full-ROM baseline: 106/106 exact modules, 11208 reproducing functions,
zero mismatches. Candidate full-ROM and final current-base proof remain pending.
The declaration ratchet still reports the external HUD int/Timer disagreement;
the whole inventory also exposes the initializer's int view. Do not bank these.
Attribution and declaration-baseline reservations remain with their existing
owners. No external ledger or shared declaration header was edited here.

## Next action

Root amends the idle producer task to reserve HUD RenderTimeTimer and the Timer
initializer, then this producer resumes the exact checkpoint. Run full affected
relocation, metadata, static and ROM checks after those repairs. A different
session must independently judge the final candidate and composed base before
root publication/integration. Local probe objects, logs and private receipt stay
under ignored build directories; none is included as a public binary artifact.
