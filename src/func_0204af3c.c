// NONMATCHING: reg triple p4a8/lo/cmd (div=26). Late-lo partials; moveq form OK;
// residual: lo→lr + e-before-c interleave + tail regperm.
struct S { int f0; unsigned f4, f8, fc; unsigned short f10, f12; };
void func_0204af3c(struct S *self) {
    int sh;
    unsigned short h12, h10;
    unsigned cmd, e, d, c, b, a, lo, t, te, ta, tb;
    unsigned fc;
    volatile unsigned slot;

    fc = self->fc;
    slot = fc;

    /* Start lo, then a/b/e partials, finish lo late (keeps double-shift). */
    t = fc << 0x1c;
    ta = fc << 0x18;
    tb = fc << 0x14;
    te = fc << 0xf;
    lo = t >> 0x1c;
    a = ta >> 0x1c;
    b = tb >> 0x1c;
    t = fc << 0x12; c = t >> 0x1e;
    t = fc << 0x10; d = t >> 0x1e;
    e = te >> 0x1f;

    cmd = (self->f4 >> 3) | (lo << 26) | 0x40000000
        | (a << 20) | (b << 23) | (c << 16) | (d << 18) | (e << 29);
    *(volatile unsigned *)0x40004a8 = cmd;

    sh = 4 - (lo == 2);
    *(volatile unsigned *)0x40004ac = self->f8 >> sh;
    *(volatile unsigned *)0x4000440 = 3;
    *(volatile unsigned *)0x4000454 = 0;
    h10 = self->f10;
    h12 = self->f12;
    *(volatile unsigned *)0x400046c = (unsigned)h10 << 12;
    *(volatile unsigned *)0x400046c = (unsigned)h12 << 12;
    *(volatile unsigned *)0x400046c = 0;
    *(volatile unsigned *)0x4000440 = 1;
}
