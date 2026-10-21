int func_02062024(unsigned char *o)
{
    unsigned short a = *(unsigned short *)(o + 4);
    if (a > 0x70) return 0;
    a = *(unsigned short *)(o + 0x16);
    if (a >= 0xa && a <= 0x3e8) {
    } else {
        return 0;
    }
    a = *(unsigned short *)(o + 0x30);
    if (a >= 1 && a <= 0xe) {
    } else {
        return 0;
    }
    {
        unsigned short c = *(unsigned short *)(o + 0x34);
        unsigned short b = *(unsigned short *)(o + 0x32);
        unsigned short d = *(unsigned short *)(o + 0xe);
        int idx = ((c + 0x20) << 2) + 0x70;
        int base = 0x14a + ((b + 0x26) << 2);
        int v = d * idx + base;
        return v <= 0x15e0;
    }
}

#include "types.h"
extern int WM_CheckStateEx(int a, int b);
extern void func_02062024(void *o);
extern void WM_SetCallbackTable(int i, int val);
extern void _ZN4CP1514FlushDataCacheEjj(u32 a, u32 b);
extern int WM_SendCommand(int a, int b, void *c);

int func_020620b0(int self, char *o) {
    int r;
    u16 t12;
    r = WM_CheckStateEx(1, 2);
    if (r != 0)
        return r;
    if (o == 0)
        return 6;
    if (*(u16 *)(o + 4) != 0) {
        if (*(int *)o == 0)
            return 6;
    }
    t12 = *(u16 *)(o + 0x12);
    if (*(u16 *)(o + 0x32) + (t12 != 0 ? 0x2a : 0) > 0x200 ||
        *(u16 *)(o + 0x34) + (t12 != 0 ? 6 : 0) > 0x200)
        return 6;
    func_02062024(o);
    WM_SetCallbackTable(7, self);
    _ZN4CP1514FlushDataCacheEjj((u32)o, 0x40);
    if (*(u16 *)(o + 4) != 0) {
        _ZN4CP1514FlushDataCacheEjj(*(u32 *)o, *(u16 *)(o + 4));
    }
    r = WM_SendCommand(7, 1, o);
    if (r == 0)
        return 2;
    return r;
}
