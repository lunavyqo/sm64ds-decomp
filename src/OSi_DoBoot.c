// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// OSi_DoBoot at 0x01ffdb28 (ITCM) -- NitroSDK, the last step of OS_ResetSystem (see
// src/OSi_ReloadRomData.c): after OSi_ReloadRomData has reloaded both binaries off the
// card, this hands off to the freshly loaded ARM9 image and never returns. The SDK ships
// it as an asm function in OS_reset.c. pret/pokediamond's copy has the same shape: IME
// off, the REG_IPCSYNC handshake, scratch clears that keep the exception-vector word at
// 0x027ffd9c, then a jump through the header's ARM9 entry with r0-r10 loaded. This older
// SDK clears two 0x80-byte blocks with func_02059d98 where the later one clears three
// smaller blocks with OSi_CpuClear32.
//
// Why this is assembly and not C, independent of the instruction list:
//   * It makes two `bl func_02059d98` calls with NO prologue and NO frame, and keeps
//     live values in r1 and ip straight across them -- the saved word at 0x027ffd9c
//     is held in ip over the first call, and the REG_IPCSYNC pointer in r1 over both.
//     That is only sound because the callee (src/unnamed/arm9/0205/func_02059d98.c, a hand-asm block
//     clear) is known to touch r0-r10 and nothing else. No compiler assumes a
//     callee's clobber set; mwccarm would spill to callee-saved registers and push.
//   * The tail is a bulk register load, `ldmia r11, {r0-r10}`, followed by `mov r11, #0`
//     and `bx ip` -- eleven registers set up as the entry state of another program and
//     then jumped to. C has no construct that assigns the register file and transfers.
//   * It never returns, so it never needs lr; `mov lr, ip` seeds the new image's lr
//     with its own entry point instead.
//
// The handshake: wait until the ARM7 puts 1 in REG_IPCSYNC's input nibble, answer with
// 1 in the output nibble (0x100), clear the two 0x80-byte scratch blocks at 0x027ffd80
// and 0x027fff80 -- preserving the word at 0x027ffd9c across the first clear -- then
// wait for the ARM7 to drop its 1 and release the register. 0x027ffe24 is the ARM9
// entry-address field of the cartridge header copy at 0x027ffe00, the same block
// src/OSi_ReloadRomData.c reads its ROM offset / RAM address / size triples out of.
//
// 0x04000208 is REG_IME; storing 0x04000000 there clears bit 0, i.e. interrupts off.
// The I/O base doubles as both the value and the store's base register.
// OSi_HW_DTCM is the SDK's own spelling of the DTCM base in OS_reset.c. The word it
// clears, 0x3fc0 + HW_DTCM_SYSRV_OFS_INTR_VECTOR, is the IRQ vector in the system
// reserve. The same pool entry is reloaded as the register-image pointer at the end,
// where the later SDK loads HW_PXI_SIGNAL_PARAM_ARM9 instead.

#include "types.h"
#include "nitro/hw/mmap_shared.h"
#include "nitro/hw/mmap_tcm.h"

#define OSi_HW_DTCM SDK_AUTOLOAD_DTCM_START

extern void func_02059d98(void *p, unsigned int n);

asm void OSi_DoBoot(void)
{
    mov ip, #0x4000000
    str ip, [ip, #0x208]
    ldr r1, =OSi_HW_DTCM
    add r1, r1, #0x3fc0
    add r1, r1, #HW_DTCM_SYSRV_OFS_INTR_VECTOR
    mov r0, #0
    str r0, [r1]
    ldr r1, =0x04000180
_wait_ack:
    ldrh r0, [r1]
    and r0, r0, #15
    cmp r0, #1
    bne _wait_ack
    mov r0, #0x100
    strh r0, [r1]
    ldr r0, =0x027ffd9c
    ldr ip, [r0]
    ldr r0, =0x027ffd80
    mov r1, #0x80
    bl func_02059d98
    ldr r0, =0x027ffd9c
    str ip, [r0]
    ldr r0, =0x027fff80
    mov r1, #0x80
    bl func_02059d98
    ldr r1, =0x04000180
_wait_release:
    ldrh r0, [r1]
    and r0, r0, #15
    cmp r0, #1
    beq _wait_release
    mov r0, #0
    strh r0, [r1]
    ldr r3, =0x027ffe00
    ldr ip, [r3, #0x24]
    mov lr, ip
    ldr r11, =OSi_HW_DTCM
    ldmia r11, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10}
    mov r11, #0
    bx ip
}

/* OSReadROMArea at 0x01ffdbd8 (ITCM) -- NitroSDK os_reset.c's static
 * OSi_ReadCardRom32: a blocking, polled read of [src, src + len) from the card
 * into dst, for OSi_ReloadRomData (src/OSi_ReloadRomData.c).
 *
 * The page-read command fetches 512-byte pages, so the transfer starts at the
 * page that holds src and drops the words that fall outside [0, len). The
 * control word is the one the boot loader left in the cartridge header copy
 * (HW_ROM_HEADER_BUF + 0x60), with its block-size field set to one page.
 */
void OSReadROMArea(u32 src, void *dst, int len) {
    vu8 *const reg_CARD_MASTERCNT = (vu8 *)(HW_REG_BASE + 0x1a1);
    vu8 *const reg_CARD_CMD = (vu8 *)(HW_REG_BASE + 0x1a8);
    vu32 *const reg_CARD_CNT = (vu32 *)(HW_REG_BASE + 0x1a4);
    vu32 *const reg_CARD_DATA = (vu32 *)(HW_REG_BASE + 0x100010);
    vu32 *const hdr_GAME_BUF = (vu32 *)(HW_ROM_HEADER_BUF + 0x60);

    enum {
        CARD_MASTER_SELECT_ROM = 0x00,
        CARD_MASTER_ENABLE = 0x80,
        CARD_CMD_READ_PAGE = 0xb7,
        CARD_CTRL_CMD_MASK = 0x07000000,
        CARD_CTRL_CMD_PAGE = 0x01000000,
        CARD_CTRL_READ = 0x00000000,
        CARD_CTRL_RESET_HI = 0x20000000,
        CARD_CTRL_START = 0x80000000,
        CARD_CTRL_READY = 0x00800000,
        CARD_ENUM_END
    };

    const u32 ctrl_start =
        (u32)((*hdr_GAME_BUF & ~CARD_CTRL_CMD_MASK) |
              (CARD_CTRL_CMD_PAGE | CARD_CTRL_READ | CARD_CTRL_START | CARD_CTRL_RESET_HI));

    int pos = -(int)(src & (0x200 - 1));

    while ((*reg_CARD_CNT & CARD_CTRL_START) != 0) {
    }
    *reg_CARD_MASTERCNT = (u32)(CARD_MASTER_SELECT_ROM | CARD_MASTER_ENABLE);

    for (src = (u32)(src + pos); pos < len; src += 0x200) {
        reg_CARD_CMD[0] = (u8)(CARD_CMD_READ_PAGE);
        reg_CARD_CMD[1] = (u8)(src >> 24);
        reg_CARD_CMD[2] = (u8)(src >> 16);
        reg_CARD_CMD[3] = (u8)(src >> 8);
        reg_CARD_CMD[4] = (u8)(src >> 0);
        reg_CARD_CMD[5] = (u8)(0);
        reg_CARD_CMD[6] = (u8)(0);
        reg_CARD_CMD[7] = (u8)(0);

        *reg_CARD_CNT = ctrl_start;
        for (;;) {
            u32 ctrl = *reg_CARD_CNT;

            if ((ctrl & CARD_CTRL_READY) != 0) {
                const u32 data = *reg_CARD_DATA;

                if ((pos >= 0) && (pos < len)) {
                    *(u32 *)((u32)dst + pos) = data;
                }

                pos += sizeof(u32);
            }

            if (!(ctrl & CARD_CTRL_START)) {
                break;
            }
        }
    }
}
