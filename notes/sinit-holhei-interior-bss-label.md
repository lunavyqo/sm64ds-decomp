# sinit fold: ov062/daHolhei_c (0x0211cf30) — interior bss labels RESOLVED

`__sinit_ov062_0211cf30` folds into `src/game/actors/d_a_holhei.cpp`.
All owned ranges byte-identical (.text/.init/.ctor/.bss), 106/106
modules, full ROM == strict stock control, `dsd check symbols` 0 NEW.

This was the same interior-bss-label wall as dossunbar/eykn/snmhed/
kurumajiku/bmb/psopt, but it resolves cleanly with the `add:` reloc
attribute that ships in the pinned dsd 0.11.0 (the blocker notes
predate it). No new mechanism is needed.

## The interior labels

`data_ov062_0211df14` and `data_ov062_0211df18` are the `.y`/`.z`
fields (+4/+8) inside the single emitted `Vector3 data_ov062_0211df10`
object. They are live `.text` load targets of this same TU:

    ov062  from:0x02116fb4 kind:load to:0x0211df14
    ov062  from:0x02116fbc kind:load to:0x0211df18

The `.init` only ever loads the `df10`/`df1c` object bases; the field
stores ride `str [r0,#4]`/`[r0,#8]` immediates, so the init section
never needs the interior names. Keeping the names in `symbols.txt`
makes `dsd check symbols` demand emitted objects at those addresses —
impossible while `df10` is one contiguous `Vector3` (byte identity
needs the single-object `.init` codegen and its auto registration).

## The fix — `to:<base> add:<off>`

Drop `df14`/`df18` from `symbols.txt` (they were never real symbols —
only interior fields the old ROM-carved `.bss` used to materialize) and
write the `.text` relocs against the `df10` base with an offset:

    from:0x02116fb4 kind:load to:0x0211df10 add:0x4 module:overlay(62)
    from:0x02116fbc kind:load to:0x0211df10 add:0x8 module:overlay(62)

`dsd delink` then resolves the destination as `df10` + addend and emits
`df10+4`/`df10+8` in the reference object — matching what the compiled
TU already emits (`*(int*)((char*)&df10 + off)` lowers to a
`df10`-symbol + addend reloc). `check symbols` has no interior rows to
check. Both gates pass.

Same recipe unblocks the sibling folds parked on this wall: emit the
containing object once (byte-exact), drop the interior `symbols.txt`
rows, and rewrite each interior `to:` as `to:<object base> add:<off>`.

## Notes specific to this fold

- Handles are `HolheiModelFilePtr`/`HolheiAnimationFilePtr`
  declaration-only `SharedFilePtr` subclasses; alias targets are the
  real `Construct`/`Destruct`/`func_02017ab4` symbols. The longer
  `HolheiAnimationFilePtr` name is load-bearing: the canonical alias
  destination is longer than the shorter `...Handle` spelling and
  `objisolate` cannot grow an emitted name.
- The two `Vector3` carry offsets are real `Vector3` globals; a
  `HolheiVecReg`/`HolheiVecRegB` sentinel ctor supplies the field
  stores, and the `Vector3` object earns the `_ZN7Vector3D1Ev`
  registration directly (no alias into the in-symtab
  `_ZN7Vector3D1Ev`, so the brq/snmbdy alias wall is dodged).
- Node temps renamed to a clean fleet-unique block `@301`–`@307`,`@310`
  via `defined_symbol_renames`.
- The `d9XX` `{method,0}` descriptors stay external `data:[]` — the
  shard only referenced them (Option-B shape, same as
  `dScMgHanachan_c`).
