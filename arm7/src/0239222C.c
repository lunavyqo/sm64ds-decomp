extern char *fn_0238E688(void);
extern unsigned short *fn_0238FA8C(unsigned short *buf, unsigned short a1, unsigned short a2,
                                 unsigned short a3, unsigned short a4);
extern int fn_023921F0(int a1, int a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(int a1);

void fn_0239222C(void) {
    unsigned short buf[0x102];
    char *base;
    int *obj;
    unsigned short *res;
    unsigned short st;
    unsigned short *v;
    base = fn_0238E688();
    base += 0x1000;
    obj = *(int **)(base + 0x610);
    res = fn_0238FA8C(buf, 0, 0, 0x14, 1);
    st = res[2];
    if (st != 0) {
        fn_023921F0(0x309, st);
        return;
    }
    *(unsigned short *)*obj = 2;
    v = fn_0238DE4C();
    v[0] = 0x1B;
    v[1] = 0;
    fn_0238DEF4((int)v);
}
