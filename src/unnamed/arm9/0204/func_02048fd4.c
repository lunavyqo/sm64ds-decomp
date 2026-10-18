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
u32 data_020821c0[16] = { 0x0ffeff7c, 0x0ffeff82, 0x0ffeff89, 0x0ffeff8f, 0x0fffff95, 0x0fffff9b, 0x0fffffa2, 0x0fffffa8, 0x0fffffae, 0x0fffffb5, 0x0fffffbb, 0x1000ffc1, 0x1000ffc7, 0x1000ffce, 0x1000ffd4, 0x1000ffda };
#pragma section forcerodata end
