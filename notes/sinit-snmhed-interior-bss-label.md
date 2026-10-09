# sinit fold blocker: ov072/daBgSnmHed_c (0x021221f8)

`__sinit_ov072_021221f8` cannot be folded into
`src/actors/daBgSnmHed_c.cpp` under the current intact-object/delink
machinery. Same class of blocker as
`notes/sinit-eykn-interior-bss-label.md`,
`notes/sinit-bmb-interior-bss-labels.md` and
`notes/sinit-dossunbar-interior-bss-labels.md` — a configured name that
must exist in ov072's linked image sits inside one emitted object, and
no mechanism can materialize interior labels there. Unlike eykn's case
(label inside an indivisible 0xc registration node), this one lands on
a record boundary, so two candidate shapes were measured; neither
passes both gates.

## Shape

Retail `.init` 0x021221f8..0x02122350 (0x158) constructs three
`SharedFilePtr`-shaped file handles, registers one destructor per
handle against 0xc registration nodes, then copies eight 8-byte PMF
literals into a 0x40 state table. A `.ctor` word at 0x0212270c points
at the initializer.

- handles: `data_ov072_02122bc4` (model, fileID 0x2af, ctor
  `func_02017acc`, dtor `func_02017ab4`), `data_ov072_02122bd4` and
  `data_ov072_02122bcc` (texseq, fileIDs 0x2ae/0x2b0, ctor
  `SharedFilePtr_Construct_TexSeq`, dtor
  `SharedFilePtr_Destruct_TexSeq` — one pool word shared by both
  registrations). Construction order is bc4, bd4, bcc — not address
  order.
- nodes: three 0xc registration nodes at 0x02122bdc/e8/f4
- table: `daBgSnmHed_c::StateFunc data_ov072_02122c00[8]` — Init/State
  pairs for states 0..3
- `.data` literals at 0x0212283c..0x0212287c

## The interior label

`data_ov072_02122c08` is the second record of the state table (table
at 0x02122c00, so +8). It is a live relocation target of arm9 main:

    arm9  from:0x02090c60 kind:load to:0x02122c08 module:overlays(71,72)

A shared spawn-slot word: the pointer means a different object per
resident overlay (`ambiguous` row). For ov071 it names a real object;
for ov072 it lands inside the table. Once this TU claims
`0x02122bc4..0x02122c40`, the name must come from the emitted objects.

## Measured shapes

1. **Single `StateFunc[8]` object** — `.init` byte-identical (0x158,
   verified end-to-end: all owned ranges IDENTICAL, 106/106 modules,
   ROM identical to strict stock control). The only failure is
   `dsd check symbols`: `data_ov072_02122c08` absent, 1 NEW error.
   This is the shape committed on the branch.
2. **Uninitialized `StateFunc[7]` overlap object** — emits the name
   with no `.init` cost, but intact-TU prep packs `.bss` sequentially
   and verifies anchors post-pack: section aligns at 0x02122c40, anchor
   requires 0x02122c08. Refused: "manifest-ordered .bss contribution
   ends at 0x02122c78, claim ends at 0x02122c40". Overlapping objects
   cannot be represented.
3. **`[1] + [7]` split** — both names land on real object starts, but
   the compiler keeps two destination bases (r2 for the singleton,
   r0 + offsets for the tail): `.init` grows by one pool word plus one
   unpaired literal load (0x160 vs 0x158). Fails the byte gate
   instead.

## The verified fold recipe (for when the wall falls)

- `SnmHedModelFilePtr`/`SnmHedTexSequenceFilePtr` declaration-only
  `SharedFilePtr` subclasses carry the `{fileID, file}` layout; the
  three extern globals are retyped to them.
- Definitions at file end in `.init` construction order: `bc4` model,
  `bd4` texseq, `bcc` texseq, then
  `daBgSnmHed_c::StateFunc data_ov072_02122c00[8]` with the eight
  class-method pointers in Init/State order.
- `undefined_symbol_aliases`: the four wrapper ctor/dtor mangled
  names + `__register_global_object` -> `func_020731dc`.
- Temporaries: nodes emit `@738`–`@740`, literals `@742`–`@756` even.
  Three collide with ov026/ov010/ov002 globals;
  `defined_symbol_renames` maps `@742→@760, @744→@761, @756→@762`.
- Manifest: `.init`/`.ctor`/`.data`/`.bss` claims + 32 `relocations`
  rows (6 calls + 17 pool loads in `.init`, 8 PMF fn words in `.data`,
  1 `.ctor` word).
- Needed tooling: interior-symbol support (emit a configured name at a
  fixed offset inside one emitted object), or an `arm9`-side module
  exemption for shared spawn-slot words.

Verification record on this branch: `tubuild verify` 20/20 MATCH,
objisolate + reloc-destinations clean; cached linkcheck with every
owned range IDENTICAL (.text 0x8f0, .init 0x158, .ctor 4, .data 0x40,
.bss NOBITS gates), module fidelity 106/106, full ROM built and
byte-identical to the strict stock control — single remaining failure
is `dsd check symbols` on `data_ov072_02122c08` (1 NEW vs the
9-error baseline).
