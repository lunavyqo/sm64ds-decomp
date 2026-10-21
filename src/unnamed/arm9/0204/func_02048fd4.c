#include "types.h"
// Copies pair i (i in 0..8) from table data_020821c0 into
// data_02099fa8 / data_02099fac.
extern u32 data_020821c0[];
extern volatile u32 data_02099fa8;
extern volatile u32 data_02099fac;

void func_02048fd4(int i)
{
    volatile u32 *tbl;
    int j;
    if (i < 0) return;
    if (i > 8) return;
    tbl = data_020821c0;
    j = i * 2;
    data_02099fa8 = tbl[j];
    j++;
    data_02099fac = tbl[j];
}

#pragma define_section forcerodata ".rodata" ".rodata" ".rodata" data_mode=far_abs
#pragma section forcerodata begin
u32 data_020821c0[16] = { 0x0000005a, 0x00000226, 0x000000c8, 0x000005dc, 0x000000c8, 0x000002ee, 0x00000096, 0x00000190, 0x00000028, 0x00000190, 0x0000005a, 0x000003e8, 0x00000032, 0x00000190, 0x000000c8, 0x000005dc };
#pragma section forcerodata end
