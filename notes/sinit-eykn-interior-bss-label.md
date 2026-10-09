# sinit fold blocker: ov071/daEykn_c (0x021228c8)

`__sinit_ov071_021228c8` cannot be folded into `src/actors/daEykn_c.cpp`
under the current intact-object/delink machinery. Documented so the next
attempt starts from the mechanism wall, not from scratch. Same class of
blocker as `notes/sinit-kurumajiku-interior-pointers.md`,
`notes/sinit-bmb-interior-bss-labels.md` and
`notes/sinit-dossunbar-interior-bss-labels.md` — a configured name that
must exist in ov071's linked image sits inside one emitted object, and
no mechanism can materialize interior labels there. As with dossunbar
the interior name is needed by *another module's* relocation.

## Shape

Retail `.init` 0x021228c8..0x02122a1c (0x154) constructs four
`SharedFilePtr`-shaped file handles, registers one destructor per handle
against 0xc registration nodes, then copies six 8-byte PMF literals into
a 48-byte state table. A `.ctor` word at 0x02122b34 points at the
initializer.

- handles: `data_ov071_02123050` (model, fileID 0x2f7),
  `data_ov071_02123038` (texseq, 0x2f8), `data_ov071_02123040` (texseq,
  0x2fb), `data_ov071_02123048` (anim, 0x2f9); ctors `func_02017acc` /
  `SharedFilePtr_Construct_TexSeq` / `_ZN13SharedFilePtr9ConstructEj`,
  dtors `func_02017ab4` / `SharedFilePtr_Destruct_TexSeq` /
  `SharedFilePtr_Destruct_Anim`. Construction order is 3050, 3038, 3040,
  3048 — not address order.
- nodes: four 0xc registration nodes at 0x02123058/64/70/7c
- table: `daEykn_c::State data_ov071_02123088[3]` — the {init, exec}
  PMF pairs wait/attack/die
- `.bss` band owned by the TU: 0x02123038..0x021230b8

The generated `__sinit_daEykn_c.cpp` is byte-identical to the retail
0x154 stream, including every `.rela.init` offset/type/addend and the
literal pool — verified with `tubuild verify` (25/25 MATCH, objisolate
clean, reloc-destinations clean) and a cached linkcheck where every
owned range (.text/.init/.ctor/.data) is byte-IDENTICAL, `.bss` passes
the NOBITS symbol/module gates, and the link itself completes. The
single remaining failure is `dsd check symbols`, below.

## The interior label

`data_ov071_0212306c` is the third word of the 0xc registration node for
the 0x02123038 texseq handle (node at 0x02123064, so +8). It is the only
configured ov071 name inside an emitted object, and it is a live
relocation target of arm9 main:

    arm9  from:0x02090bcc kind:load to:0x0212306c module:overlays(71,73,74)

That arm9 word is a shared spawn-slot table entry: the stored pointer
0x0212306c means a different object per resident overlay (`ambiguous`
row). For ov073 the same address is `g_profile_KING_DONKETU`; for ov071
it lands inside the node. ov073/ov074 still gap-link the region so their
names survive; once ov071's TU claims `0x02123038..0x021230b8`, the name
must come from the emitted object — which cannot emit it.

## The verified fold recipe (for when the wall falls)

All 25 owned functions match byte-for-byte with this shape:

- Three declaration-only `SharedFilePtr` subclasses carry the
  `{fileID, file}` layout: `EyknModelFileHandle` (ctor `func_02017acc`,
  dtor `func_02017ab4`), `EyknTexSeqFileResHandle` (ctor
  `SharedFilePtr_Construct_TexSeq`, dtor `SharedFilePtr_Destruct_TexSeq`)
  and `EyknAnimationFileHandle` (ctor `_ZN13SharedFilePtr9ConstructEj`,
  dtor `SharedFilePtr_Destruct_Anim`). The four extern globals are
  retyped to them.
- Definitions at file end in `.init` construction order: `3050` model,
  `3038` texseq, `3040` texseq, `3048` anim, then
  `daEykn_c::State data_ov071_02123088[3]` with the six class-method
  pointers. `#pragma defer_codegen off` keeps emission ROM-ascending.
- `undefined_symbol_aliases`: the six wrapper ctor/dtor mangled names +
  `__register_global_object` -> `func_020731dc`.
- Temporaries: nodes emit `@1128`–`@1131`, literals `@1133`–`@1143`
  (odd). Nine collided with ov002/ov090 globals;
  `defined_symbol_renames` maps `@1128→@1100, @1129→@1101, @1131→@1102,
  @1133→@1103, @1135→@1104, @1137→@1105, @1139→@1106, @1141→@1107,
  @1143→@1108` (`@1130` was already free).
- Manifest: `production_mode: intact-object`, `.init`/`.ctor`/`.data`/
  `.bss` claims + 33 `relocations` rows (8 calls + 18 pool loads in
  `.init`, the `.ctor` word, 6 PMF record targets in `.data`).

## Why it cannot ship today

`config/arm9/overlays/ov071/symbols.txt` needs `data_ov071_0212306c`
defined in ov071's linked image (it resolves arm9's delink name and is
audited by `dsd check symbols`), but the emitted 0xc node object cannot
expose a symbol at +8. Every avenue is closed — the dossunbar note
enumerates them (`kind:label(arm)` tested live, `partition_symbols`/
`storage_alias`/`emitted_storage_address` are `_ZTV`-gated, gap objects
can't cover claimed ranges, LCF emits only boundary symbols, no
symbol+addend relocs, no `.s` enrollment path). This instance is
narrower — one label instead of three — but identical in mechanism.

## If the wall ever falls

The fold is otherwise ready and byte-verified; the committed state on
`sinit/ov071-eykn` carries the full recipe. Any of the fixes listed in
the dossunbar note unblocks this fold too: interior labels materialized
inside an emitted object, symbol+addend `to:` targets in relocs.txt, or
a containment kind in symbols.txt.
