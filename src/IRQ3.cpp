//cpp
/* IRQ CPSR primitives, the IRQ-state init helper, and the two followers
 * (arm9 .text 0x02059cb4..0x02059d8c): ARMProcessorMode (CPSR mode-bit
 * reader, hand-asm, C linkage under its name) and CP15::WaitForInterrupt
 * (wait-for-interrupt op, a CP15 method the ROM places in this run).
 *
 * IRQ::RestoreAll/DisableAll/Restore/Disable/Enable are hand-asm
 * primitives: `mrs`/`msr` read and write the CPSR and no C construct
 * compiles to them. Bit 0x80 is IRQ, 0x40 is FIQ -- the whole
 * difference between the plain and the `All` variants. Each returns
 * the OLD state of those bits so the caller can restore it. The name
 * is the tree class name plus the numeric suffix style already used
 * in the tree, per Juan 2026-10-11 (E13).
 *
 * `#pragma defer_codegen off` makes mwccarm emit .text in source
 * order: without it the hand-asm primitives emit eagerly while the C
 * helper flushes at the end of the TU, so no source order reproduces
 * the ROM order. With it the definitions run ROM-ascending and the
 * object matches the ROM. The address-named helper keeps C linkage
 * under its name at its address position. This TU builds under the
 * default 2004/b56 pin.
 */
#include "types.h"

#pragma defer_codegen off

// @symbol func_02059cb4
extern "C" {
extern unsigned int _ZN3IRQ11DisableIRQsEj(u32 mask);

extern volatile u16 data_020a644c;    /* 0x020a644c */
extern u32 data_020a6450[2];          /* 0x020a6450 */

void func_02059cb4(void) {
    if (data_020a644c != 0)
        return;
    u32 *data = data_020a6450;
    data_020a644c = 1;
    data[0] = 0;
    data[1] = 0;
    _ZN3IRQ11DisableIRQsEj(4);
}
}

namespace IRQ {

// @symbol _ZN3IRQ6EnableEv
asm unsigned int Enable(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// @symbol _ZN3IRQ7DisableEv
asm unsigned int Disable(void)
{
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// @symbol _ZN3IRQ7RestoreEj
asm unsigned int Restore(unsigned int state)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}

// @symbol _ZN3IRQ10DisableAllEv
asm unsigned int DisableAll(void)
{
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}

// @symbol _ZN3IRQ10RestoreAllEj
asm unsigned int RestoreAll(unsigned int state)
{
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}

}

// @symbol ARMProcessorMode
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was
// assembly in the original (CPSR read), so there is no C to decompile
// it to -- the asm block is the faithful source. Probed as C++
// (extern "C") byte-identical, so it folds here under its name.
extern "C" {
asm void ARMProcessorMode(void) { mrs r0, cpsr; and r0, r0, #0x1f; bx lr }
}

namespace CP15 {

// @symbol _ZN4CP1516WaitForInterruptEv
/* Halt the core until an interrupt arrives (c7,c0,4 is the
 * wait-for-interrupt op). The value written is ignored by the hardware
 * but the register still has to be materialized. HAND-ASM PRIMITIVE:
 * `mcr p15` is a coprocessor access and no C construct compiles to it.
 * A CP15 method living in another system's source file: the ROM puts
 * it here, so this TU holds it under its existing mangled name. */
void WaitForInterrupt(void)
{
    unsigned int v = 0;
    asm { mcr p15,0,v,c7,c0,4 }
}

}
