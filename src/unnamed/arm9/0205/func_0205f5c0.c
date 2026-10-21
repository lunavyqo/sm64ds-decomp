extern int func_0205f5fc(int a, void *b, int *c);
extern void func_0205ff08(void);
extern void func_0205ff00(void);

int func_0205f5c0(int a) {
    int local;
    int r = func_0205f5fc(a, (void *)func_0205ff00, &local);
    if (r != 0) return r;
    func_0205ff08();
    return local;
}


int func_0205ff20(int a, int b, int c);
void func_0205f8b0(int param);

struct D020a8114 { int w0; int w1; int w2; };
extern struct D020a8114 data_020a8114;

int func_0205f5fc(int a, int b, int c) {
    if (!func_0205ff20(a, b, c)) return 1;
    data_020a8114.w1 = b;
    data_020a8114.w2 = c;
    func_0205f8b0((a & 0xff) | 0x3006600);
    return 0;
}
