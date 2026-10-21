extern void func_0204dab4(void *a0, void *a1, void *a2, int a3, void *a4);
extern void func_ov007_020c3fe4(void *obj);
extern void func_ov007_020be7a8(void);
extern int *data_ov007_02104bc0;

struct Elem {
    short f0;
    short f2;
    short f4;
    unsigned char f6 : 2;
    unsigned char f6b : 6;
    unsigned char pad7;
    void *f8;
};

void func_ov007_020be684(void *a0, char *ctx)
{
    int i;
    int r3;
    int *q;
    char *e;
    char *o;

    q = *(int**)(*(int**)(ctx + 0xc));
    if (q[2] != 0)
        r3 = *(unsigned char*)((char*)q + 0x14);
    else
        r3 = 4;
    r3 = r3 & 0xff;
    func_0204dab4(a0, (void*)data_ov007_02104bc0[1],
                  (void*)**(int**)ctx, r3, (void*)&func_ov007_020be7a8);

    for (i = 0; i < **(int**)ctx; i++) {
        o = (char*)((int**)*(int**)(ctx + 8))[i];
        e = (char*)(data_ov007_02104bc0[1]) + i * 0xc;
        *(int*)(*(int*)o + 8) = (*(int*)(e + 8) != 0) ? 1 : 0;
        *(int*)*(int*)o = *(short*)e << 12;
        *(int*)(*(int*)o + 4) = *(short*)(e + 2) << 12;
        *(unsigned char*)(*(int*)o + 0x14) = ((struct Elem*)e)->f6;
        *(unsigned char*)(*(int*)o + 0x15) = (*(short*)(e + 4) << 4) >> 0xc;
        if (*(int*)(e + 8) != 0) func_ov007_020c3fe4(o);
    }
}

struct G {
    char pad24[0x24];
    int f24; // 0x24
    char pad28[0x30 - 0x28];
    int f30; // 0x30
    char padec[0xec - 0x34];
    int fec; // 0xec
};

extern void func_ov007_020bfd70(int a, int b, int *p0, int *p1);
extern struct G *data_ov007_0210342c;
extern unsigned short data_ov007_020d7c58[];

void func_ov007_020be7a8(int a0, unsigned short *out1, unsigned short *out2, int *out3) {
    int loc0;
    int loc1;
    struct G *g;

    g = data_ov007_0210342c;
    func_ov007_020bfd70(g->f30, a0, &loc0, &loc1);

    g = data_ov007_0210342c;
    if (g->f24 == 4) {
        loc1 = loc1 + 0x14b;
    } else {
        loc1 = loc1 + data_ov007_020d7c58[g->fec];
    }

    *out1 = (unsigned short)loc0;
    *out2 = (unsigned short)loc1;
    *out3 = 0xfff;
}
