# ARM7 request-call agreement

Base: `8080bc51e2fe17a8983cdca2de513ccf32388550`, the unchanged source head of
upstream PR #3758, from `lunavyqo/sm64ds-decomp:batch/arm7-1012-8`.
This is a separate follow-up; it does not modify that branch or the existing
fork PR #10. Queue task: `arm7-call-arity1018`.

## Problem and source change

`fn_0238FA8C` takes a halfword buffer and four unsigned-halfword arguments.
Its definition consumes the fifth parameter. Of its three source callers:

- `02390604.c` already has the correct declaration and five-argument call.
- `02392134.c` already passes five arguments, but declares the last two as int.
- `0239222C.c` declares and passes only four arguments, using a separate volatile
  stack field to manufacture the value consumed as the fifth argument.

The correction gives the two affected declarations the definition's signature.
`0239222C.c` uses an ordinary unsigned-short buffer of the same 0x204-byte extent
and explicitly calls `fn_0238FA8C(buf, 0, 0, 0x14, 1)`. The volatile field and
its containing frame struct are removed. `02392134.c` selects its halfword mode
with a plain if/else before the existing five-argument call. All three callers
now agree with the callee on this interface.

The direct typed ternary and typed-local ternary experiments for `02392134`
emitted 196 bytes rather than 188. The plain branches emit the original object
exactly; no compiler flag, pragma, dummy operation, or volatile workaround was
added. This observation is not a claim that every other natural spelling fails.

## Runtime evidence

The original `0239222C` starts at runtime address 0x027c41a4. Its instruction
at 0x027c41bc stores 1 at [sp], then it sets
r0 to sp+4, r1/r2 to zero and r3 to 0x14, then calls 0x027c1a04. The callee
pushes eight bytes and reads the halfword at [sp+8], storing it into the request
record at +0x16. This establishes the missing stack argument independently of
the source declarations. The sibling `02390604` call already supplies the same
`0, 0, 0x14, 1` argument sequence explicitly.

## Producer proof

Use the branch's production ARM7 build, not the ARM9/overlay build:

```powershell
python arm7/build.py
```

Compiler: 2004/b56, production flags `-O4,s -proc arm7tdmi -interworking -fp soft
-enum int -lang c99 -char signed`; linker: 1.2/sp2p3. Inputs, flags, enrollment,
absolute-symbol map and expected image hash are unchanged.

Both original-base and candidate builds reproduce all 150,308 ARM7 bytes:
SHA-1 `0efbc2d43d0a7ae4a087472fd0ec7ca2875ec865`. The build has 822 enrolled
source rows and 142,088 source bytes; other bytes are the existing retail gaps.
This is image equality, not a claim that the assembly stand-ins are decompiled C.

| Changed source | Storage range | Runtime range | Exact bytes | Checked call relocations |
|---|---|---|---:|---:|
| 02392134.c | 0x02392134..0x023921f0 | 0x027c40ac..0x027c4168 | 188 | 7 |
| 0239222C.c | 0x0239222c..0x023922a8 | 0x027c41a4..0x027c4220 | 124 | 5 |

The final production objects are byte-for-byte equal to their untouched base
objects, including the existing exception sections. Focused linked-byte checks
also verify the 312 function bytes and all twelve actual branch destinations;
there are no masked relocation words in that comparison. The full image build
then verifies their actual composition with every other enrolled function.
Private producer artifacts live under `build/arity/` and `build/arity-scout/`.
Independent verification must bind its findings to the published immutable SHA.

## Scope and remaining reconstruction

Only these two caller files and this handoff change. No new shared declaration
header is introduced for one existing three-caller interface. The callee,
other caller, enrollment, symbol map and build tools are unchanged. Existing
address-derived names, raw accesses and unrelated declaration debt remain.
This does not resolve the separate uninitialized-read or register-pinning
findings from the ARM7 review, and makes no ARM9 source/TU ownership claim.

Publish a separate PR against the fork's ARM7 branch after independent review.
Ordinary upstream source checks do not validate `arm7/`; report the actual local
ARM7 proof and any CI coverage limits explicitly. No merge is authorized.
