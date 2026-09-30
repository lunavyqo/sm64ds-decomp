# Super Mario 64 DS Decompilation

[![Discord Server][discord-badge]][discord]

[discord]: https://discord.gg/YpReERF4e3
[discord-badge]: https://img.shields.io/discord/1520811338568569112?color=7289DA&logo=discord&logoColor=ffffff

> **Looking for the PC port?** [Download it here.](https://tangos.dev/downloads)

A work-in-progress decompilation of Super Mario 64 DS

This repo holds source code and tooling. It contains no ROM and no Nintendo assets.
Everything here runs against a cartridge dump you supply yourself, which stays on your
machine and is git-ignored.

New here? Start with **[CONTRIBUTING.md](CONTRIBUTING.md)**, and if you review or
merge PRs read **[MERGE.md](MERGE.md)**.

## Progress

<!-- progress:start -->
```
Functions  ██████████████████████████████  99.8%   11,368 / 11,389
Code size  ██████████████████████████████  98.9%   2,212,808 / 2,238,108 bytes
```
<!-- progress:end -->

<!-- tiers:start -->
```
MATCHED    ██████████████████████████████  99.8%   11,368 / 11,389 functions
           of which 121 are byte-exact assembly (hand-written in the original, not C)
CONVERTED  █████████░░░░░░░░░░░░░░░░░░░░░  29.0%   3,302 / 11,386 functions
LINKED     ████████████████████████████░░  93.5%   10,595 / 11,328 matched TUs
```
<!-- tiers:end -->

- **MATCHED** — compiles to the ROM's exact bytes. This is the bar above and the
  treemap, and the same standard the N64 `sm64` project holds to.
- **CONVERTED** — source a person can read without the ROM open beside them.
  Matching doesn't require readable code, so this moves on its own and lags behind.
- **LINKED** — matched code that reaches the [PC port](port/)'s binary.

![Decompilation progress treemap](docs/progress-treemap.svg)

Hover any function for its name, address, and status on the
[interactive treemap](https://tangosdev.github.io/sm64ds-decomp/). Run
`python tools/tiers.py` for the full breakdown, and see
[notes/converted-tier.md](notes/converted-tier.md) for how each tier is defined.

## Setup

You supply your own cartridge dump.

```sh
pip install ndspy capstone pyelftools
# get mwccarm per notes/setup-mwccarm.md, then:
python tools/unpack.py "path/to/your-own-sm64ds.nds"
```

Full setup — both halves of the mwccarm install, the dsd toolkit, and unpacking your
ROM — is in [CONTRIBUTING.md](CONTRIBUTING.md) and
[notes/setup-mwccarm.md](notes/setup-mwccarm.md).

## How matching works

The matching compiler is pinned to **mwccarm 2004/b56** with these flags:

```sh
-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e -gccext,on -msgstyle gcc
```

C++ sources use `-lang c++` and `-Cpp_exceptions off`; `tools/match.py` makes both
substitutions itself for a file whose first line is `//cpp`.

Every function is compiled and compared to the ROM byte-for-byte, relocation-aware.
Nothing counts as matched until that check passes, and near misses are banked in the
database rather than thrown away. A small number of routines were hand-written in
assembly in the original and carry that source; the rule for which ones qualifies is
in [notes/asm-policy.md](notes/asm-policy.md).

## Contributing

Pick a function, write C or C++, verify it compiles to the same bytes as the ROM, then
open a pull request. One function or a small related group per PR is ideal. Use only
your own legally dumped ROM, and never commit it.

Most matches land through [tangOS Console](https://tangos.dev/downloads), the free
desktop app built for this repo, which hands out work, verifies candidates against the
ROM, and collects your matches into a formatted PR.

## Credits

Symbol names and struct knowledge build on community reverse-engineering work. See
[CREDITS.md](CREDITS.md) for the full list, and the contributor chart for per-person
match counts. The rule is import knowledge, write code: you may use known symbol names
and field offsets, but all source must be written from scratch against your own ROM.

Function contributions: [RyanCopley](https://github.com/RyanCopley) hand-matched a set of
functions across ov002, ov006, arm9, and ov034 (PR #1), including the first functions in
ov034 and several that had resisted the automated passes.
[andrewboudreau](https://github.com/andrewboudreau) has hand-matched a large and growing set of
functions across arm9 and many overlays (PRs #2, #45, #48, and the #50 through #60 constructor and
static-initializer batches), contributed codegen notes on boolean materialization and
predicated-select shapes (PR #49), and reported the scheduler bug fixed in #61.
[Moundistz](https://github.com/Moundistz) contributed 3 matched functions and 8 nonmatching
floor entries, plus a pass of placeholder-to-resolved callee renames across the arm9 corpus.
[lunavyqo](https://github.com/lunavyqo) has hand-matched across ov001 and a wide spread of
the scene overlays, and carries the class translation-unit cleanup work, promoting a folded
actor TU and then clearing the one-function leftovers it strands.
[ruspecial](https://github.com/ruspecial) has hand-matched large batches across ov002, ov006
and the arm9 BIOS SVC stubs, banked 174 near-misses with the C++ exception-handling and
split-symbol triage notes behind them (PR #806), and contributed oracle-verified host copies
for the two Bowser ov060 holes on the port side (PR #1505).

Tooling contributions: [webheadvr](https://github.com/webheadvr) made the relocation
symbol resolver module-aware, fixing wrong-overlay symbol picks where overlay address
ranges overlap. [andrewboudreau](https://github.com/andrewboudreau) added a relocation
destination audit and an opt-in strict-relocs check to the verify path (PR #47).

The `tools/coddog.py` fuzzy opcode-similarity scheduler was inspired by
[Chris Lewis's writeup](https://blog.chrislewis.au/the-long-tail-of-llm-assisted-decompilation/)
on LLM-assisted decompilation and the Coddog similarity tool it describes.

## Legal and scope

The original work here — the source, the tooling, the notes — is MIT licensed, see
[LICENSE](LICENSE), and grants no rights to any Nintendo material. The one documented
exception is the `chaos-data` branch, which carries annotated disassembly text of
still-unmatched functions so contributors can pick up work without a full local setup.
It is text, not bytes or assets, and each function's disassembly leaves it once matched.
