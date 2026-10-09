# sinit fold blocker: ov015/daObjBk_Dossunbar_c (0x02113048)

`__sinit_ov015_02113048` cannot be folded into
`src/game/actors/d_a_obj_bk_dossunbar.cpp` under the current
intact-object/delink machinery. Documented so the next attempt starts
from the mechanism wall, not from scratch. Same class of blocker as
`notes/sinit-kurumajiku-interior-pointers.md` and
`notes/sinit-bmb-interior-bss-labels.md`, with one difference that makes
this instance strictly worse: the interior names are needed by *other
modules'* relocations, not just this TU's own `.init`/`.text` image.

## Shape

Retail `.init` 0x02113048..0x02113264 (0x21c) constructs four
`SharedFilePtr`-shaped file handles, registers one destructor per handle
against 0xc registration nodes, then copies two seven-entry PMF state
tables from `.data` literals. A `.ctor` word at 0x02113418 points at the
initializer.

- handles: `data_ov015_021149a4` (model, fileID 0x58d), `data_ov015_021149ac`
  (clsn, 0x58e), `data_ov015_021149b4` (model, 0x58b),
  `data_ov015_0211499c` (clsn, 0x58c); ctors `func_02017acc` /
  `func_02017b4c`, dtors `func_02017ab4` / `SharedFilePtr_Destruct_Clsn`
- nodes: four 0xc registration nodes at 0x021149bc/9c8/9d4/9e0
- tables: `PMF data_ov015_02114a24[7]` (handler column, copied first) and
  `StateEntry data_ov015_021149ec[7]` (enter column, copied second);
  each record is one 8-byte `{ptr, adj}` PMF
- `.bss` band owned by the TU: 0x0211499c..0x02114a5c

The generated `__sinit_d_a_obj_bk_dossunbar.cpp` is byte-identical to
the retail 0x21c stream, including every `.rela.init` offset/type/addend
and the literal pool — verified with `tubuild verify` (25/25 MATCH,
objisolate clean, reloc-destinations clean) and a linkcheck where every
owned range (.text/.init/.ctor/.data/.bss) is IDENTICAL, 106/106 modules
byte-exact, and the full ROM build byte-identical. The single remaining
failure is `dsd check symbols`, below.

## The interior labels

`data_ov015_021149ec` emits as one 0x38 `.bss` object
(`StateEntry[7]` — the `.init` copy loads a single destination pool word
and stores through base+offset, so retail emitted one object, not four).
Three configured names point inside it:

    data_ov015_021149f0 addr:0x021149f0   (record[0].adj, +4)
    data_ov015_021149fc addr:0x021149fc   (record[2].adj, +0x10)
    data_ov015_02114a18 addr:0x02114a18   (record[5].adj, +0x2c)

They are the `.adj` words of enter-table records 0, 2 and 5, and they
are live relocation targets in two other modules:

    arm9  from:0x02090944 kind:load to:0x021149fc module:overlays(15,16,20,21)
    arm9  from:0x02090948 kind:load to:0x02114a18 module:overlays(15,16,20,21)
    arm9  from:0x02090d7c kind:load to:0x021149f0 module:overlays(15,16,20,21)
    ov021 from:0x02113660 kind:load to:0x021149f0 module:overlay(21)
    ov021 from:0x0211366c kind:load to:0x021149fc module:overlay(21)

`overlays(15,16,20,21)` are the four dossun-bar sibling overlays sharing
this address window; each carries an identically-shaped table at these
addresses in its own image. ov016/ov020/ov021 still gap-link the range,
so their own `data_ovNNN_...` interior names survive in their modules'
links. The moment ov015's TU claims the range, ov015's names must come
from the emitted object — which cannot emit them.

## The verified fold recipe (for when the wall falls)

All 25 owned functions match byte-for-byte with this shape:

- The 14 PMF-record handlers are methods of `daObjBk_Dossunbar_c`
  (`func_ov015_02111ce0`..`func_ov015_02111fac` keeping their address
  names inside the class), and the PMF-table installer
  `func_ov015_02111fb8` is methodized too — object first arg, only
  called by class methods. All `self->` bodies become `this->`.
- File handles are declaration-only subclasses carrying the
  `{fileID, file}` layout: `DossunbarModelFilePtr` (ctor
  `func_02017acc`, dtor `func_02017ab4`) and `DossunbarCollisionFilePtr`
  (ctor `func_02017b4c`, dtor `SharedFilePtr_Destruct_Clsn`), each with
  a `u32` ctor and declared-undefined dtor. `undefined_symbol_aliases`
  maps the four mangled ctor/dtor names plus `__register_global_object`
  -> `func_020731dc`.
- Definitions at file end in `.init` order: handles `9a4, 9ac, 9b4, 99c`,
  then `PMF data_ov015_02114a24[7]`, then `StateEntry
  data_ov015_021149ec[7]` (element order 9ec-table last, matching the
  copy order).
- Consumers read through the typed arrays (`mState` indexes
  `data_ov015_021149ec`, the installer walks `PMF{ptr,adj}` words).
- `.bss` definition order in symbols.txt/object output is address
  ascending regardless of `.init` copy order.
- Temporaries: registration nodes emit `@730`–`@733`, the 14 PMF
  literals `@735`–`@761` (odd). Nine collided with ov002/ov010/ov026
  globals; `defined_symbol_renames` maps `@743→@763, @745→@764,
  @747→@765, @749→@768, @751→@770, @753→@774, @755→@775, @757→@776,
  @759→@777` (all unused everywhere, same length).
- Manifest: `production_mode: intact-object`, `.init`/`.ctor`/`.data`/
  `.bss` section claims with relocations, `undefined_symbol_aliases`
  above, `defined_symbol_renames` above, deadstrip policy for the
  sinit entry point.

## Why it cannot ship today

`config/arm9/overlays/ov015/symbols.txt` needs the three interior names
defined in ov015's linked image (they resolve arm9/ov021 delink names
and are audited by `dsd check symbols`), but a single emitted 0x38
object cannot expose symbols at +4/+0x10/+0x2c. Every avenue is closed:

- **split emitted objects at 9f0/9fc/a18**: changes the `.init`
  schedule — four destination pool words instead of one (+0xc), `.init`
  grows 0x21c -> 0x228 and the base+offset store stream becomes
  per-object sequences. Bytes diverge.
- **`kind:bss` rows**: `dsd delink` resolves names but
  `check symbols` needs the names defined in the linked ELF;
  `bss(size=...)` on the owner row does not emit interior names.
- **`kind:label(arm)` rows**: tested — `check symbols` still fails;
  labels only materialize inside *delinked* objects (per the bmb note),
  and the emitted `d_a_obj_bk_dossunbar.o` replaces the delinked object
  in the link.
- **gap-object coverage**: `_dsd_gap@ov015_*.o` emits configured symbols
  only outside claimed ranges; once the `.bss` claim owns
  0x021149ec..0x02114a24 the gap object cannot name the interior.
- **`partition_symbols`/`manifest_storage_alias_rows`/`storageAlias`/
  `emitted_storage_address`/`address_point_bias`**: gated to `_ZTV`
  owners (`validated_vtable_partition_symbols`); they re-name or re-bias
  emitted vtable symbols, not synthesize interior data labels.
- **`defined_symbol_renames`**: renames an already-emitted symbol to a
  manifest-owned name; cannot create interior names.
- **`symbol_binding_rewrites`**: STB_LOCAL/GLOBAL only, not
  names/addresses.
- **LCF absolute assignments**: `dsd lcf` emits only
  `{MODULE}_{SECTION}_{START,END}` boundary symbols; no configured
  symbol-injection mechanism exists.
- **`relocs.txt` symbol+addend targets** (`to:<base> add:<off>`): not in
  the grammar on this base — `sinit-pool.md` already records this as the
  needed fix for the identical ov004-psopt blocker.
- **delinked twin carrying the names**: the link consumes only the
  compiled object for claimed sources; `complete` serves ROM bytes for
  the deleted handwritten file, and its emitted `.bss` contribution
  would double-claim the range anyway.

The original handwritten file worked precisely because `complete`
serves retail `.init` bytes directly while the gap object supplies every
`.bss` name — the fold removes the only shape that can name the
interior addresses.

## If the wall ever falls

The fold is otherwise ready and byte-verified. Needed, any one of:

- a manifest mechanism that declares interior labels inside an emitted
  object (`kind:label`-style names materialized into the emitted
  object's symbol table, or `check symbols` accepting delinked-object
  label coverage), or
- `to:` reloc targets as symbol+addend in `relocs.txt`, or
- a delink-time alias/containment kind in `symbols.txt`.

Then apply the recipe above and delete
`src/unnamed/ov015/__sinit_ov015_02113048.c` for real. The folded state
is committed on this branch (`sinit/ov015-dossunbar`) — `.init`,
`.ctor`, `.data`, `.bss` and all 25 functions byte-match; only the
three interior names are missing from the linked binary.
