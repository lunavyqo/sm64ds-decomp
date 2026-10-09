# sinit fold blocker: ov102/daBmb_c (0x0214de70)

`__sinit_ov102_0214de70` cannot be folded into `src/actors/daBmb_c.cpp`
under the current intact-object/delink machinery. Documented so the next
attempt starts from the mechanism wall, not from scratch. Same class of
blocker as `notes/sinit-kurumajiku-interior-pointers.md`, with one important
difference noted below: `kind:label` fixes `dsd delink` here; only
`dsd check symbols` remains unsatisfiable.

## Shape

Retail `.init` 0x0214de70..0x0214dfac (0x13c) constructs two animation file
handles and four carry-offset vectors, then registers all six objects with
`func_020731dc` (__register_global_object), plus a `.ctor` word at
0x0214e0a8 pointing at the initializer.

- handles: `data_ov102_0214e9c0` / `data_ov102_0214e9c8` (8 bytes each,
  `SharedFilePtr`-shaped, one `u32 fileID` arg each)
- nodes: six 0xc registration nodes at 0x0214e9d0..0x0214ea18
- vectors: `data_ov102_0214ea18/ea24/ea30/ea3c` — 0xc each, `Vector3`-shaped,
  all registered against `_ZN7Vector3D1Ev`
- `.bss` band owned by the TU: 0x0214e9c0..0x0214ea48 (0x88)

The generated `__sinit_daBmb_c.cpp` is byte-identical to the retail 0x13c
stream, including all 22 `.rela.init` offsets/types/addends and the literal
pool — verified with `tubuild verify` (35/35 MATCH) and a stock-equal ROM
build (`arm9_ov102.bin` byte-identical; five owned ranges all IDENTICAL).

## The verified fold recipe (for when the wall falls)

Two details were hard-won; both are needed:

1. **Store-order control.** Retail stores v0 (`data_ov102_0214ea18`) with
   *free* ordering — the y-value `rsb` chain materializes and hoists above
   the x-store — while v1..v3 store x,y,z strictly in source order with a
   flat `ldr r0; mov r1; str` stream (no arg hoist). No single constructor
   body gives both. The winning shape is one wrapper type with two
   constructor overloads:

   ```cpp
   struct BmbVec3 : Vector3 {
       BmbVec3(Fix12i x_, Fix12i y_, Fix12i z_) { x = x_; y = y_; z = z_; }
       BmbVec3(Fix12i x_, Fix12i y_, Fix12i z_, int) {
           volatile BmbVec3 *p = this;   // pins store order to source order
           p->x = x_; p->y = y_; p->z = z_;
       }
       using Vector3::~Vector3;          // see (2)
   };
   ```

   `data_ov102_0214ea18` takes the 3-arg overload (free schedule, y-chain
   floats first — matches retail v0). `ea24/ea30/ea3c` take the 4-arg
   overload with a dummy `0` — the volatile self-pointer forces the flat
   `ldr;mov r1;str` serial store stream retail shows. Both overloads inline
   fully; the dummy arg emits no code.

2. **Dtor registration without an alias.** The `using Vector3::~Vector3;`
   declaration makes the registration reference `_ZN7Vector3D1Ev` directly.
   This TU also emits a *local* `_ZN7Vector3D1Ev`/`D2Ev` definition (taken
   from `Vector3` locals elsewhere in the TU), so an
   `undefined_symbol_aliases` entry would collide; the `using` decl makes
   the name resolve through the existing `deadstrip-duplicate`/`deadstrip`
   policies with no alias row at all.

   The file handles need the same retype pattern other folded sinits use:
   `struct BmbAnimationFileHandle : SharedFilePtr` with a `u32` ctor and
   declared-undefined dtor — `SharedFilePtr.h` is the method-only real class.

   Definitions go at file end in sinit order:
   `data_ov102_0214e9c0`, `data_ov102_0214e9c8`, then the four vectors.
   Consumer `func_ov102_0214b53c` spells the field loads as
   `data_ov102_0214ea18.y` / `.z` (produces `data_ov102_0214ea18`+4/+8
   relocated words — identical relocated bytes to the retail interior-label
   targets).

## Why it cannot ship today

`config/arm9/overlays/ov102/symbols.txt` carries interior rows for the
first vector's fields:

    data_ov102_0214ea1c addr:0x0214ea1c   (ea18.y, +4 inside the 0xc object)
    data_ov102_0214ea20 addr:0x0214ea20   (ea18.z, +8)

`func_ov102_0214b53c`'s retail image reads them via two interior `ldr`s
(0x0214b974 -> 0x0214ea1c, 0x0214b978 -> 0x0214ea20), so the names must
exist for the delinker and for `check symbols`. But `data_ov102_0214ea18`
must emit as a 0xc `Vector3`-shaped class — only that emits the v0
registration against `_ZN7Vector3D1Ev` — and 4+4+4 tiling is the only way
to place object symbols at +4/+8. `data_ov102_0214ea18` must therefore be
both 0xc and 4 bytes: impossible.

Every avenue is closed:

- **emitted `int` objects at ea1c/ea20**: `.bss` sections lay out
  sequentially; inside a 0xc emitted object's span they cannot exist, and
  trailing them grows `.bss` 0x88 -> 0x90 (claim mismatch) while still
  landing at the wrong addresses.
- **`kind:bss` rows**: delink fails — "No symbol found for relocation
  from 0x0214b974 ... to 0x0214ea1c". `bss(size=0xc)` on the owner row does
  not enable containment resolution.
- **`kind:label(arm)` rows**: `dsd delink` succeeds — dsd emits
  `data_ov102_0214ea1c`/`ea20` as STT_NOTYPE interior labels at offsets
  0x5c/0x60 in the *delinked* `delinks/src/actors/daBmb_c.o`. But the
  emitted `daBmb_c.cpp.o` replaces that object in the link, and no
  mechanism injects the labels into an emitted object. `dsd check symbols`
  then fails: "Symbol 'data_ov102_0214ea1c' in overlay 102 at 0x0214ea1c
  not found in linked binary".
- **gap-object coverage**: `_dsd_gap@ov102_*.o` emits configured symbols
  only in unowned ranges; ea1c..ea24 sits inside the `complete` `.bss`
  claim, so the gap object skips it (regenerated gap objects confirm:
  `data_ov102_0214ea48/ea58/ea68/ea78` present, ea1c/ea20 absent).
- **split/non-`complete` `.bss` claims**: `section_contribution` lays every
  same-named input section into each claim's image — a claim subset is not
  expressible, and the 0xc object cannot split across a boundary anyway.
- **`partition_symbols`/`storage_alias`/`emitted_storage_address`/
  `address_point_bias`**: gated to `_ZTV` owners
  (`validated_vtable_partition_symbols`), and they verify emitted symbols,
  not synthesize them.
- **`defined_symbol_renames`**: renames an already-emitted symbol to a
  manifest-owned name; cannot create interior names.
- **`symbol_binding_rewrites`**: STB_LOCAL/GLOBAL only, not names/addresses.
- **zero-sized `int x[0]`**: emits no usable bss symbol.
- **4-byte wrapper class**: an `int`-membered class could tile ea1c/ea20,
  but v0's registration needs `_ZN7Vector3D1Ev` on a 0xc object — and the
  TU's local `_ZN7Vector3D1Ev` def makes even a hypothetical UNDEF-alias
  impossible.
- **`__declspec(section)` / per-function retarget**: mwccarm does not
  retarget emitted sections; `retarget_text_section` handles the .init
  case but no equivalent exists for symbol placement.

The original handwritten file worked precisely because `complete` serves
retail `.init` bytes directly while the gap object supplies every `.bss`
name — the fold removes the only shape that can name the interior
addresses.

## If the wall ever falls

The fold is otherwise ready and byte-verified. Needed, any one of:

- a way to declare `kind:label`-style symbols at interior offsets of an
  emitted object (i.e., `check symbols` accepting delinked-object label
  coverage, or an emitted-object interior-symbol record), or
- `to:` reloc targets as symbol+addend in `relocs.txt`, or
- a delink-time alias/containment kind in `symbols.txt`.

Then: apply the recipe above — `BmbAnimationFileHandle : SharedFilePtr`
pair + `BmbVec3 : Vector3` with the two ctor overloads and
`using Vector3::~Vector3`, objects at file end in sinit order, consumer
spelled through `data_ov102_0214ea18.y`/`.z`, `.text/.init/.ctor/.data/
.bss` claims, `undefined_symbol_aliases` for the veneer/fileID pair,
`deadstrip`/`deadstrip-duplicate`/`compiler_only_output` for the local
`_ZN7Vector3D1Ev`/`D2Ev` copies, `defined_symbol_renames` mapping the
generated `__sinit_daBmb_c.cpp`/`.p__sinit_daBmb_c.cpp` to the retail
names, six `@NNN` node temps renamed to the `data_ov102_0214e9d0`..ea10
configured names — and delete the handwritten anchor.
