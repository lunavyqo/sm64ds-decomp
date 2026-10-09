# sinit fold blocker: ov077/daTgz_c (0x0212749c)

`__sinit_ov077_0212749c` cannot be folded into `src/actors/daTgz_c.cpp`
under the current intact-object/delink machinery. Same class of blocker
as the interior-label notes on the parked sibling folds
(`sinit/ov071-eykn`, `sinit/ov072-snmhed`, `sinit/ov015-dossunbar`)
— a configured name that
must exist in ov077's linked image sits inside one emitted object, and
no mechanism can materialize interior labels there. Two such names here,
and unlike eykn/snmhed the arm9 side does not survive: the shared-slot
relocation resolves its import name from *this* overlay's own symbols
rows, which are the ones that go missing.

## Shape

Retail `.init` 0x0212749c..0x021275fc (0x160) constructs one
`SharedFilePtr`-shaped file handle, registers its destructor against a
0xc registration node, then copies twelve 8-byte PMF literals into a
0x60 state table. A `.ctor` word at 0x021277d0 points at the
initializer.

- handle: `data_ov077_02127c14` (BCA anim, fileID 0x42b, ctor
  `_ZN13SharedFilePtr9ConstructEj`, dtor `SharedFilePtr_Destruct_Anim`)
- node: `@1200`-shaped 0xc registration node at 0x02127c1c
- table: `TgzStatePmf data_ov077_02127c28[12]` — six {enter, update}
  pairs over the TU's `extern "C" int func_ov077_*(daTgz_c*)` state
  functions; `func_ov077_02125e94` indexes it by pairs
- `.data` literals at 0x021278e8..0x02127948, in a cartridge order that
  is not table order (they are standalone named objects, emitted
  straight into `.data`, not anonymous compiler temps)

The generated `__sinit_daTgz_c.cpp` is byte-identical to the retail
0x160 stream and every `.rela.init` offset/type/addend matches —
`tubuild verify` reports 34/34 MATCH with objisolate clean, and the
cached linkcheck verifies all five owned ranges (`.text` 5864B, `.init`
352B, `.ctor` 4B, `.data` 96B, `.bss` 0x74 NOBITS) as IDENTICAL.

## The interior labels

Two configured ov077 names sit inside the emitted table:

    arm9  from:0x02090af4 kind:load to:0x02127c5c module:overlays(77,79)
    arm9  from:0x02090af8 kind:load to:0x02127c40 module:overlays(77,79)

`data_ov077_02127c40` is +0x18 into the table (a record boundary);
`data_ov077_02127c5c` is +0x34 — *inside* record 6's second word, not a
boundary any array split can express. Both rows are `ambiguous` shared
spawn-slot words: in ov079's space the same addresses are the real
objects `g_profile_BATANKING` / `g_profile_BATAN`.

The arm9 gap object's import name is resolved from the *first* module
of the `overlays(77,79)` group — ov077 — so it imports
`data_ov077_02127c5c` / `data_ov077_02127c40`. Once this TU claims
`.bss 0x02127c14..0x02127c88`, no emitted object defines them (the
array is one 0x60 symbol), the slots resolve to 0, and the arm9 module
differs by 8 bytes. Deleting the rows entirely is worse: `dsd delink`
refuses the arm9 reloc outright ("No symbol found for relocation ...
in overlay 77" — it never falls back to the sibling module's name).

For contrast: snmhed's identical-shaped slot picked ov071's
`g_profile_SPIDER` — the first-listed module had a name that survives
in the link, so its arm9 module stayed identical. ov077 is the first
module of its group, so there is no surviving sibling name to borrow.

## Measured shapes

1. **Single `TgzStatePmf[12]` object** — `.init` byte-identical, all
   owned ranges IDENTICAL, 34/34 functions MATCH, but arm9 differs by
   8 bytes and `dsd check symbols` reports 2 NEW errors. This is the
   shape committed on the branch.
2. **Record-boundary pieces** (`c28[3]` + a `{rec3..5, rec6.fn}` +
   `{rec6.adj, rec7..11}` tiling) — emits all three names, but each
   object's initializer writes through its own base symbol: extra
   literal-pool loads break `.init` bytes and the relocation names
   differ from retail's single `data_ov077_02127c28` base. Fails the
   byte gate instead. (snmhed's `[1]+[7]` variant measured the same
   +8-byte cost on a single boundary.)
3. **Row deletion** — removes the check-symbols rows; `dsd delink`
   then fails the arm9 relocations because the first module of
   `overlays(77,79)` has no configured name at the targets.

## What would unblock it

A mechanism that materializes sub-symbols at interior offsets of one
emitted object — the shape `partition_symbols`/`storage_alias` already
implement for `_ZTV` retained vtables — applied to a `.bss` claim, or
an arm9 relocation model that resolves a shared `overlays(a,b)` import
through whichever module actually defines a name. Until then the
initializer remains foldable in source (this branch carries it, byte
exact in every owned range) but the full-ROM gate cannot pass.
