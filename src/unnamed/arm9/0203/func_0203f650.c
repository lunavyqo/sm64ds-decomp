extern void (*data_020a0f90)(short *);

void func_0203f650(int x)
{
    short v[2];
    void (*fn)(short *) = data_020a0f90;
    if (fn == 0)
        return;
    if (x > 0) {
        v[0] = 0;
        v[1] = (short)x;
    } else {
        v[0] = (short)x;
        v[1] = 0;
    }
    fn(v);
}

extern int data_02099e20[];
void func_0203f6a4(int v) {
    data_02099e20[0] = v;
}

extern int func_020621b8(void *p);
extern void Crash(void);
extern void func_0203f920(void);
void func_0203f6b4(void) {
    if (func_020621b8((void *)func_0203f920) == 2)
        return;
    Crash();
}

extern short data_020a0f18;
extern void func_0203f6b4(void);
extern void func_0203f6a4(int v);

void func_0203f6e8(short v) {
    data_020a0f18 = v;
    func_0203f6b4();
    func_0203f6a4(2);
}

#include "types.h"
extern void func_0203f6e8(s16 v);
extern int func_0203f818(int arg);
extern int func_02059640(void);
extern int __aeabi_idiv(int a, int b);

extern u16 data_020a0f34;
extern u16 data_020a0fd0[];
extern u8 data_020a0ef4;
extern s16 data_020a0f18;

struct S {
    u16 f0, f2, f4, f6, f8, fa;
};

void func_0203f714(struct S *s) {
    u16 a, cur, v;
    int r;
    if (s->f2 != 0) {
        func_0203f6e8(-4);
        return;
    }
    a = s->fa;
    cur = data_020a0f34;
    v = s->f8;
    if (a < cur) {
        data_020a0f34 = a;
        data_020a0ef4 = 1;
        data_020a0fd0[0] = v;
    } else if (a == cur) {
        data_020a0fd0[data_020a0ef4] = v;
        data_020a0ef4++;
    }
    r = func_0203f818((u16)(v + 1));
    if (r == 1) {
        int n = func_02059640();
        int idx = n % data_020a0ef4;
        data_020a0f18 = data_020a0fd0[idx];
        func_0203f6e8(data_020a0f18);
    } else if (r == 2) {
        func_0203f6e8(-3);
    }
}

extern unsigned short func_02061960(void);
extern int func_020634fc(void *a, int b, int c, int d, unsigned short e);
extern void func_0203f920(void);

int func_0203f818(int arg) {
    unsigned int n = arg;
    unsigned int mask = func_02061960();
    while (((1 << (n - 1)) & mask) == 0) {
        n = (unsigned short)(n + 1);
        if (n > 0xe)
            return 1;
    }
    if (func_020634fc((void *)func_0203f920, 3, 0x11, n, 0x1e) != 2)
        return 2;
    return 0;
}

extern int func_0203f818(int a);
extern void func_0203f6e8(short v);
extern void func_0203f6a4(int v);
extern short data_020a0f18;
extern short data_020a0f34;

void func_0203f8a4(void) {
    int r;
    data_020a0f18 = 0;
    data_020a0f34 = 0x65;
    r = func_0203f818(1);
    if (r == 1) {
        func_0203f6e8(-9);
    } else if (r == 2) {
        func_0203f6e8(-3);
    } else {
        func_0203f6a4(1);
    }
}

#include "types.h"
extern void func_0203f650(int x);
extern void func_0203f8a4(void);
extern void func_0203f714(void);
extern void func_0203f6a4(int v);
extern s16 data_020a0f18;
extern int data_020a0f90;

void func_0203f920(u16 *o) {
    switch (o[0]) {
    case 0:
        if (o[1] != 0)
            func_0203f650(-2);
        else
            func_0203f8a4();
        break;
    case 0x1e:
        func_0203f714();
        break;
    case 2:
        func_0203f6a4(3);
        func_0203f650(data_020a0f18);
        data_020a0f90 = 0;
        break;
    default:
        func_0203f650(data_020a0f18);
        break;
    }
}

extern void func_0203f650(int x);
extern int func_02062240(int a0, int a1, int a2);
extern void func_0203f6a4(int v);
extern int data_02099e20;
extern int data_020a0f90;
extern int func_0203f920(void);

void func_0203f9d4(int a0, int a1) {
    data_020a0f90 = a1;
    if (data_02099e20 != 3) {
        func_0203f650(-10);
        return;
    }
    if (func_02062240(a0, (int)&func_0203f920, 1) != 2) {
        func_0203f650(-1);
        return;
    }
    func_0203f6a4(0);
}
