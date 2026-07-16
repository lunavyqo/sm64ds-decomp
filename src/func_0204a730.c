// NONMATCHING: size 0x6ec vs 0x6fc; large schedule residual (div=999 size-diff). Compiles;
// particle/callback table path draft from nearmiss DB.
typedef unsigned int u32;
typedef int s32;
typedef unsigned short u16;
typedef short s16;
typedef unsigned char u8;
typedef signed char s8;
typedef long long s64;

extern void func_0204c584(void *self, void *arg);
extern void func_0204c304(void *node, void *self, void *arg);
extern void *func_0204d8e8(void *listHead, void *node);
extern void func_0204d9a0(void *list, void *node);

typedef void (*Cb1)(void *, void *, u32);
typedef void (*Cb2)(u32, void *, void *);

typedef struct {
    Cb1 func;
    u32 data;
} Ent1;

typedef struct {
    Cb2 func;
    u32 data;
} Ent2;

extern void func_0204d440(void *, void *, u32);
extern void func_0204d294(void *, void *, u32);
extern void func_0204d1b4(void *, void *, u32);
extern void func_0204d150(void *, void *, u32);
extern void func_0204d104(void *, void *, u32);
extern void func_0204d0b8(void *, void *, u32);

#pragma opt_common_subs off
#pragma opt_propagation off
void func_0204a730(void *arg0, void *self)
{
    char *sb;
    char *player;
    char *cfg;
    u32 flags;
    u16 thresh;
    u16 count2;
    volatile s32 spC;
    s32 n;
    Ent1 table1[4];
    Ent1 tableB[4];
    void *var_r4;
    void *var_r5;
    u8 spArr[2];
    s32 acc[3];

    sb = *(char **)((char *)self + 0x18);
    cfg = *(char **)(sb + 0x14);
    player = *(char **)(sb + 0);
    flags = *(u32 *)(player + 0);
    thresh = *(u16 *)(player + 0x28);
    count2 = *(u16 *)(sb + 0x1c);
    spC = (s32)(*(u8 *)(player + 0x32)) + 0x180;
    n = 0;

    if (thresh == 0 || (u16)(*(u16 *)((char *)self + 0x38)) < thresh) {
        if ((u16)(*(u16 *)((char *)self + 0x38)) % (u8)(*(u8 *)((char *)self + 0x58)) == 0) {
            s32 f = *(s32 *)((char *)self + 0x1c);
            if (((u32)(f << 0x1f) >> 0x1f) == 0 && ((u32)(f << 0x1e) >> 0x1f) == 0) {
                func_0204c584(self, (char *)arg0 + 0x14);
            }
        }
    }

    if (((u32)(flags << 0x17) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 4);
        table1[n].func = func_0204d440;
        table1[n].data = (u32)((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f);
        n += 1;
    }
    if (((u32)(flags << 0x16) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 8);
        if (((u32)(*(u16 *)(t + 8) << 0x1f) >> 0x1f) == 0) {
            table1[n].func = func_0204d294;
            table1[n].data = (u32)((u32)(*(u16 *)(t + 8) << 0x1e) >> 0x1f);
            n += 1;
        }
    }
    if (((u32)(flags << 0x15) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 0xc);
        table1[n].func = func_0204d1b4;
        table1[n].data = (u32)((u32)(*(u16 *)(t + 2) << 0x17) >> 0x1f);
        n += 1;
    }
    if (((u32)(flags << 0x14) >> 0x1f) != 0) {
        char *t = *(char **)(sb + 0x10);
        if (((u32)(*(s32 *)(t + 8) << 0xf) >> 0x1f) == 0) {
            table1[n].func = func_0204d150;
            table1[n].data = (u32)((u32)(*(s32 *)(t + 8) << 0xe) >> 0x1f);
            n += 1;
        }
    }

    var_r4 = *(void **)((char *)self + 8);
    if (var_r4 != 0) {
        s32 sp10 = (s32)((u32)(flags << 0x10) >> 0x1f);
        s32 sp14 = (s32)((u32)(flags << 0xf) >> 0x1f);
        s32 zero = 0;
        void *next;
        do {
            char *nodeC = (char *)var_r4;
            u16 ratio;
            next = *(void **)(nodeC + 0);
            ratio = *(u16 *)(nodeC + 0x2e);
            s32 i;

            spArr[0] = (u8)((s32)(*(u16 *)(nodeC + 0x3e) * ratio) >> 8);
            spArr[1] = (u8)((s32)(*(u16 *)(nodeC + 0x3c) * ratio) >> 8);

            i = zero;
            if (n > 0) {
                do {
                    table1[i].func(var_r4, sb, spArr[table1[i].data]);
                    i += 1;
                } while (i < n);
            }

            acc[0] = zero;
            acc[1] = zero;
            acc[2] = zero;

            if (sp10 != 0) {
                *(s32 *)(nodeC + 8) = *(s32 *)((char *)self + 0x20);
                *(s32 *)(nodeC + 0xc) = *(s32 *)((char *)self + 0x24);
                *(s32 *)(nodeC + 0x10) = *(s32 *)((char *)self + 0x28);
            }

            if ((s32)count2 > 0) {
                s32 j = zero;
                do {
                    Ent2 *tbl2 = *(Ent2 **)(sb + 0x18);
                    tbl2[j].func(tbl2[j].data, var_r4, &acc[0]);
                    j += 1;
                } while (j < (s32)count2);
            }

            *(u16 *)(nodeC + 0x36) = (u16)(*(u16 *)(nodeC + 0x36) + *(s16 *)(nodeC + 0x38));
            {
                s32 *pX = (s32 *)(nodeC + 0x20);
                s32 *pY = (s32 *)(nodeC + 0x24);
                s32 *pZ = (s32 *)(nodeC + 0x28);
                s32 *pPosX = (s32 *)(int)(((long long)(int)(nodeC + 0x14)) & 0xFFFFFFFFFFFFFFFFLL);
                s32 *pPosY = (s32 *)(int)(((long long)(int)(nodeC + 0x18)) & 0xFFFFFFFFFFFFFFFFLL);
                s32 *pPosZ = (s32 *)(int)(((long long)(int)(nodeC + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL);

                *pX = (*pX * spC) >> 9;
                *pY = (*pY * spC) >> 9;
                *pZ = (*pZ * spC) >> 9;
                *pX = *pX + acc[0];
                *pY = *pY + acc[1];
                *pZ = *pZ + acc[2];
                *pPosX = *pPosX + *pX + *(s32 *)((char *)self + 0x2c);
                *pPosY = *pPosY + *pY + *(s32 *)((char *)self + 0x30);
                *pPosZ = *pPosZ + *pZ + *(s32 *)((char *)self + 0x34);
            }

            if (sp14 != 0) {
                u16 life = *(u16 *)(nodeC + 0x2e);
                s32 a = (s32)(*(u16 *)(nodeC + 0x2c)) << 0xc;
                s32 b = ((s32)(*(u8 *)(cfg + 0xd)) + 1) << 0xc;
                s32 r1 = (s32)(((s64)a * b + 0x800) >> 0xc);
                s32 r0 = ((s32)life << 0xc) - (r1 >> 8);
                if (life != 0 && r0 >= 0 && ((r0 >> 0xc) % (u8)(*(u8 *)(cfg + 0xe))) == 0) {
                    func_0204c304(var_r4, self, (char *)arg0 + 0x14);
                }
            }

            *(u16 *)(nodeC + 0x2e) = (u16)(*(u16 *)(nodeC + 0x2e) + 1);
            if ((u16)(*(u16 *)(nodeC + 0x2e)) > (u16)(*(u16 *)(nodeC + 0x2c))) {
                func_0204d9a0((char *)arg0 + 0x14, func_0204d8e8((char *)self + 8, var_r4));
            }

            var_r4 = next;
        } while (next != 0);
    }

    if (((u32)(flags << 0xf) >> 0x1f) != 0) {
        s32 n2 = 0;
        u16 cflags;

        cflags = *(u16 *)cfg;
        if (((u32)(cflags << 0x1e) >> 0x1f) != 0) {
            tableB[n2].func = func_0204d104;
            tableB[n2].data = 0;
            n2 += 1;
        }
        cflags = *(u16 *)cfg;
        if (((u32)(cflags << 0x1d) >> 0x1f) != 0) {
            tableB[n2].func = func_0204d0b8;
            tableB[n2].data = 0;
            n2 += 1;
        }
        cflags = *(u16 *)cfg;
        var_r5 = *(void **)((char *)self + 0x10);
        if (((u32)(cflags << 0x1f) >> 0x1f) == 0) {
            count2 = 0;
        }

        if (var_r5 != 0) {
            s32 zero2 = 0;
            void *next;
            do {
                char *nodeC = (char *)var_r5;
                u16 e2c;
                u16 e2e;
                s32 j;

                e2e = *(u16 *)(nodeC + 0x2e);
                next = *(void **)(nodeC + 0);
                e2c = *(u16 *)(nodeC + 0x2c);
                spArr[0] = (u8)(((s32)(e2e << 8)) / (s32)e2c);

                j = zero2;
                if (n2 > 0) {
                    do {
                        tableB[j].func(var_r5, sb, spArr[0]);
                        j += 1;
                    } while (j < n2);
                }

                acc[0] = zero2;
                acc[1] = zero2;
                acc[2] = zero2;

                if (((u32)(cflags << 0x1a) >> 0x1f) != 0) {
                    *(s32 *)(nodeC + 8) = *(s32 *)((char *)self + 0x20);
                    *(s32 *)(nodeC + 0xc) = *(s32 *)((char *)self + 0x24);
                    *(s32 *)(nodeC + 0x10) = *(s32 *)((char *)self + 0x28);
                }

                if ((s32)count2 > 0) {
                    s32 k = zero2;
                    do {
                        Ent2 *tbl2 = *(Ent2 **)(sb + 0x18);
                        tbl2[k].func(tbl2[k].data, var_r5, &acc[0]);
                        k += 1;
                    } while (k < (s32)count2);
                }

                *(u16 *)(nodeC + 0x36) = (u16)(*(u16 *)(nodeC + 0x36) + *(s16 *)(nodeC + 0x38));
                {
                    s32 *pX = (s32 *)(nodeC + 0x20);
                    s32 *pY = (s32 *)(nodeC + 0x24);
                    s32 *pZ = (s32 *)(nodeC + 0x28);
                    s32 *pPosX = (s32 *)(int)(((long long)(int)(nodeC + 0x14)) & 0xFFFFFFFFFFFFFFFFLL);
                    s32 *pPosY = (s32 *)(int)(((long long)(int)(nodeC + 0x18)) & 0xFFFFFFFFFFFFFFFFLL);
                    s32 *pPosZ = (s32 *)(int)(((long long)(int)(nodeC + 0x1c)) & 0xFFFFFFFFFFFFFFFFLL);

                    *pX = (*pX * spC) >> 9;
                    *pY = (*pY * spC) >> 9;
                    *pZ = (*pZ * spC) >> 9;
                    *pX = *pX + acc[0];
                    *pY = *pY + acc[1];
                    *pZ = *pZ + acc[2];
                    *pPosX = *pPosX + *pX + *(s32 *)((char *)self + 0x2c);
                    *pPosY = *pPosY + *pY + *(s32 *)((char *)self + 0x30);
                    *pPosZ = *pPosZ + *pZ + *(s32 *)((char *)self + 0x34);
                }

                *(u16 *)(nodeC + 0x2e) = (u16)(*(u16 *)(nodeC + 0x2e) + 1);
                if ((u16)(*(u16 *)(nodeC + 0x2e)) > (u16)(*(u16 *)(nodeC + 0x2c))) {
                    func_0204d9a0((char *)arg0 + 0x14, func_0204d8e8((char *)self + 0x10, var_r5));
                }

                var_r5 = next;
            } while (next != 0);
        }
    }

    *(u16 *)((char *)self + 0x38) = (u16)(*(u16 *)((char *)self + 0x38) + 1);

    {
        void (*cb)(void *) = *(void (**)(void *))((char *)self + 0x68);
        if (cb == 0) {
            return;
        }
        cb(self);
    }
}
