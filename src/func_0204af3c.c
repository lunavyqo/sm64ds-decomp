// NONMATCHING: pack-schedule/regperm wall (div=38). Correct size/frame; lo via lsl+lsr;
// cmd packing order and lo==2 materialization float early vs ROM; pool reg colors differ.
typedef unsigned int u32;
typedef unsigned short u16;

struct S {
    int f0;
    unsigned int f4;
    unsigned int f8;
    unsigned int fc;
    unsigned short f10;
    unsigned short f12;
};

void func_0204af3c(struct S *self)
{
    volatile unsigned int slot;
    unsigned int fc = self->fc;
    unsigned int t, lo, a, b, c, d, e;
    unsigned int cmd;
    int sh;
    unsigned int f10, f12;

    slot = fc;

    t = fc << 0x1c;
    lo = t >> 0x1c;

    t = fc << 0x18;
    a = t >> 0x1c;
    t = fc << 0x14;
    b = t >> 0x1c;
    t = fc << 0x12;
    c = t >> 0x1e;
    t = fc << 0x10;
    d = t >> 0x1e;
    t = fc << 0xf;
    e = t >> 0x1f;

    cmd = (self->f4 >> 3) | (lo << 26);
    cmd |= 0x40000000;
    cmd |= a << 20;
    cmd |= b << 23;
    cmd |= c << 16;
    cmd |= d << 18;
    cmd |= e << 29;
    *(volatile unsigned int *)0x40004a8 = cmd;

    sh = 4 - (lo == 2);
    *(volatile unsigned int *)0x40004ac = self->f8 >> sh;
    *(volatile unsigned int *)0x4000440 = 3;
    *(volatile unsigned int *)0x4000454 = 0;
    f10 = self->f10;
    f12 = self->f12;
    *(volatile unsigned int *)0x400046c = f10 << 12;
    *(volatile unsigned int *)0x400046c = f12 << 12;
    *(volatile unsigned int *)0x400046c = 0;
    *(volatile unsigned int *)0x4000440 = 1;
}
