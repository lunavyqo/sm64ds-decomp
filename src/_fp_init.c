// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// _fp_init at 0x01ffafd4 (ITCM) -- resets the floating-point runtime state that the
// soft-float block above it (0x01ff8000-0x01ffafd0: the double add/multiply, the float
// and double conversions, and the integer division helpers) reads and updates.
//
// It zeroes the status word func_0207322c points at (the word __ieee_status and
// __fp_status rewrite), then the five words at +4..+0x14 of the block func_02073238
// points at; word +0 is left alone. Those five words are the exception handlers
// src/cstd4.cpp's func_0206de14 dispatches through, one per bit of 26-30 in its flags
// argument (0x04000000 -> +4 ... 0x40000000 -> +0x14), so this leaves the status clear
// and no handler installed.
//
// The name is from the call site. _start (src/_start.c), the ARM9 startup code, ends with
// `bl 01ffafd4; bl NitroStartUp; bl __call_static_initializers`, then loads NitroMain and jumps
// to it with lr = 0xffff0000. NitroSDK's crt0 ends `_start` with exactly that sequence:
// `bl _fp_init; bl NitroStartUp; bl __call_static_initializers`, then `ldr r1, =NitroMain`
// with lr = HW_RESET_VECTOR (the NitroSDK crt0.c in pret's pokediamond). The body
// does the job ARM's runtime gives _fp_init ("initialize the floating-point status
// register before performing any floating-point operations"). Pokemon Diamond links a
// runtime whose _fp_init is a bare `bx lr`, so only the name and the slot are shared with
// it, not the body.
//
// Assembly in the original, like the whole runtime block it belongs to: it keeps r0 live
// across a `bl` with only `{lr}` pushed, relying on the callee returning its result in r0
// and clobbering nothing else, and it reuses the second callee's return value as the base
// for five stores without ever re-materialising it. mwccarm compiling equivalent C emits a
// callee-saved spill and a real frame; this has neither.

// __ieee_status and __fp_status, folded in here with _fp_init, are assembly for the
// same reason: each preserves its arguments across the status-word accessor call by
// pushing r0-r2 and ip, a register set no compiler-generated prologue saves. __ieee_status
// is ARM DUI0475's read-modify-write of the fplib status word; __fp_status wraps the same
// operation in the older status-word layout's bit repack.

extern unsigned int *func_0207322c(void);
extern void *func_02073238(void);


asm void _fp_init(void)
{
    stmdb sp!, {lr}
    bl func_0207322c
    mov r1, #0
    str r1, [r0]
    bl func_02073238
    mov r1, #0
    str r1, [r0, #4]
    str r1, [r0, #8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    ldmia sp!, {lr}
    bx lr
}

asm unsigned int __ieee_status(unsigned int mask, unsigned int flags)
{
    stmdb sp!, {r0, r1, r2, r12, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, r12, lr}
    ldr r2, [r3]
    bic r12, r2, r0
    eor r12, r12, r1
    str r12, [r3]
    mov r0, r2
    bx lr
}

asm unsigned int __fp_status(unsigned int mask, unsigned int flags)
{
    and r2, r0, #0x1f
    and r3, r0, #0x1f0000
    orr r0, r2, r3, lsr #8
    and r2, r1, #0x1f
    and r3, r1, #0x1f0000
    orr r1, r2, r3, lsr #8
    stmdb sp!, {r0, r1, r2, r12, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, r12, lr}
    ldr r2, [r3]
    bic r12, r2, r0
    eor r12, r12, r1
    str r12, [r3]
    and r0, r2, #0x1f
    and r2, r2, #0x1f00
    orr r0, r0, r2, lsl #8
    orr r0, r0, #0x40000000
    bx lr
}
