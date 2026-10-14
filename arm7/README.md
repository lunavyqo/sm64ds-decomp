# ARM7

The ARM7 binary (`arm7.bin`, 0x24B24 bytes, SHA-1 in `arm7.sha1`) built from source.
There was no ARM7 precedent in this tree, so it lives in its own directory and follows
the arm9 rules where they apply: one function per file named by storage address, the
`// NONMATCHING` banners from `notes/asm-policy.md`, and retail bytes for anything that
is not built from source (as the ROM build does with dsd gap bytes).

    python arm7/build.py [--base extracted/dsd/arm7/arm7.bin]

Needs `tools/mwccarm` (or `ARM7_MW`) and the dsd extract. The retail image is only read;
nothing extracted is written into the tree. Output goes to `build/arm7/`.

## Layout

- `src/<storage>.c` - one function each. `<storage>` is the address in the ARM7 binary as
  loaded (0x02380000 + file offset); the comment header gives the runtime address.
  The `wram` part (storage 0x02380168..0x0238E088) runs at 0x037F8000, the `main` part
  (0x0238E088..0x023A4B0C) at 0x027C0000.
- `src/02380000.s` (crt0) and `src/02388130.s` (Thumb SWI veneers) - mwasmarm sources.
- `swap.txt` - the enrolled functions: storage, size, symbol, source, region. Only these
  are compiled and linked at their address. A file in `src/` with no row is not enrolled.
- `syms.txt` - absolute runtime addresses for symbols that source refers to but that are
  not defined by an enrolled object.

## Kinds of file

- Real C, no asm of any kind: enrolled, counts as matched.
- `// NONMATCHING (ASM-PRIMITIVE)` + `// HAND-ASM PRIMITIVE`: enrolled assembly tagged
  as an original primitive. Status-register, banked-register, SWP and SWI instructions
  provide assembly evidence; a tag alone does not establish the origin of an ordinary
  veneer or runtime routine. These entries are separate from the pure C count below.
- `// NONMATCHING (ASM-STANDIN)`: enrolled byte-exact assembly retained pending C
  matching or stronger original-source evidence. These are not pure C matches; three
  currently contain privileged context operations despite the historical stand-in tag.
  Upstream tools read them as plain `NONMATCHING` drafts.
- `// NONMATCHING` C: the closest C attempt, not enrolled (the build keeps retail bytes),
  banked in `nearmiss/db.jsonl`.

## Verified coverage

The current enrollment has 824 rows and reproduces the complete 150,308-byte retail
image (SHA-1 `0efbc2d43d0a7ae4a087472fd0ec7ca2875ec865`). Of those rows,
822 compile `.c` files and two assemble the crt0/Thumb packs. The compiled files
include 763 pure C sources, 26 tagged ASM-PRIMITIVE sources, and 33 ASM-STANDIN
sources. Their combined 0x22FA4 bytes include assembly; the remaining 0x1B80 bytes
are retail gaps. A matching whole-image hash does not establish native C coverage.

Under the PR's 828-entry counting convention, the pure C sources cover 765 entries.
Two of those entries are IDA tails within `02387C98.c` and `0238F9BC.c`, rather than
additional C functions. The 63 other entries are the 26 tagged primitives, 33
stand-ins, and four unenrolled C drafts. Tags describe the existing inventory and
are not independent proof that every tagged primitive was originally assembly.

In particular, three stand-ins (`02381484`, `023814B8`, and `0238BB20`) contain
privileged status-register or banked-register operations. The other 30 still need
C matching or stronger origin evidence; the explicit stack operations in the
`0238B11C` bit-bang routine leave its original source form uncertain. This recovery
does not relabel any of those sources or declare a compiler limitation.

The existing AH21 source `02388798.c` is now enrolled with its complete 0x224-byte
extent: 0x208 bytes of code followed by a 0x1C-byte literal pool. It ends exactly at
the next enrolled function, `023889BC`. Its C body is unchanged; the former draft
extent omitted the pool. See `notes/experiments/arm7-existing-match1014.json` for
the pinned compiler, linked relocation destinations, and recovery provenance.

The 0x3C-byte `0238E0E4.c` now expresses the sixteen-sample average and
classification call in genuine C. Its two literal-pool destinations and complete
linked extent match retail. Enrollment and build settings are unchanged. See
`notes/experiments/arm7-average-native1014.json` for the exact inputs, complete
ARM7 image proof, and remaining reconstruction limits.

The AH54 draft `023A0CE0.c` is now matched and enrolled at its full 0x278-byte
extent, including the 0x0C-byte literal pool. A native four-bit C bitfield
assignment reproduces the previously different nibble-update register sequence.
Its caller `023A11E0.c` now declares the actual two-argument interface and uses
one eight-halfword buffer for seven initialized fields and the final untouched
halfword included in the sibling path's 16-byte copy. The complete caller
output remains unchanged. See `notes/experiments/arm7-receive-native1014.json`
for exact call destinations, the full-image replay, and remaining partial layouts.
