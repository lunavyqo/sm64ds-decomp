#include "types.h"
typedef struct EchoChan {
    int l;
    int r;
} EchoChan;

typedef struct EchoWork {
    char pad[0xc];
    EchoChan ch[24];
} EchoWork;

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 addr, u32 size);

void func_020497a0(s16* p0, s16* p1, u32 len, EchoWork* st)
{
    int n = len >> 1;
    int j;
    int pos = 0;
    int rem;
    int shift;
    int i;

    while (n > pos + 24) {
        for (j = 0; j < 24; j++) {
            int idx = j + pos;
            int v0 = p0[idx] + st->ch[j].l;
            int v1 = p1[idx] + st->ch[j].r;
            int s0 = v0;
            int s1;
            if (v0 < -0x8000) {
                s0 = -0x8000;
            } else if (v0 >= 0x8000) {
                s0 = 0x7fff;
            }
            p0[idx] = s0;
            s1 = v1;
            if (v1 < -0x8000) {
                s1 = -0x8000;
            } else if (v1 >= 0x8000) {
                s1 = 0x7fff;
            }
            p1[idx] = s1;
            st->ch[j].l = (v1 + 1) >> 2;
            st->ch[j].r = (v0 + 1) >> 2;
        }
        pos += 24;
    }

    rem = n - pos;
    for (n = 0; n < rem; n++) {
        int idx = n + pos;
        int v0 = p0[idx] + st->ch[n].l;
        int v1 = p1[idx] + st->ch[n].r;
        if (v0 < -0x8000) {
            v0 = -0x8000;
        } else if (v0 >= 0x8000) {
            v0 = 0x7fff;
        }
        p0[idx] = v0;
        if (v1 < -0x8000) {
            v1 = -0x8000;
        } else if (v1 >= 0x8000) {
            v1 = 0x7fff;
        }
        p1[idx] = v1;
    }

    shift = 24 - rem;
    for (i = 0; i < shift; i++) {
        st->ch[i].l = st->ch[i + rem].l;
        st->ch[i].r = st->ch[i + rem].r;
    }

    for (i = 0; i < rem; i++) {
        int idx = i + pos;
        int dst = i + 24 - rem;
        st->ch[dst].l = (p1[idx] + 1) >> 2;
        st->ch[dst].r = (p0[idx] + 1) >> 2;
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p0, len);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)p1, len);
}

typedef short s16;

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(void *addr, unsigned int size);

#pragma opt_strength_reduction off
#pragma opt_common_subs off

void func_020499bc(s16 *a0, s16 *a1, unsigned int a2, int *a3) {
    s16 local[2];
    int half = a2 >> 1;
    int i;

    for (i = 0; i < 2; i++) {
        int idx = i + half - 2;
        int d = a0[idx] - a1[idx];
        if (d < -0x8000)
            d = -0x8000;
        else if (d >= 0x8000)
            d = 0x7fff;
        local[i] = d;
    }

    for (i = half - 1; i >= 2; i--) {
        int d = a0[i - 2] - a1[i - 2];
        if (d >= 0) {
            int hi = d + a0[i];
            a0[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            {
                int lo = a1[i] - d;
                a1[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            }
        } else {
            int lo = d + a0[i];
            a0[i] = (lo >= -0x8000) ? (s16)lo : -0x8000;
            {
                int hi = a1[i] - d;
                a1[i] = (hi < 0x8000) ? (s16)hi : 0x7fff;
            }
        }
    }

    for (i = 1; i >= 0; i--) {
        int v = a0[i] + *(int *)((char *)(a3 + i) + 0xc);
        if (v < -0x8000)
            v = -0x8000;
        else if (v >= 0x8000)
            v = 0x7fff;
        a0[i] = v;
        {
            int w = a1[i] - *(int *)((char *)(a3 + i) + 0xc);
            if (w < -0x8000)
                w = -0x8000;
            else if (w >= 0x8000)
                w = 0x7fff;
            a1[i] = w;
        }
    }

    for (i = 0; i < 2; i++) {
        *(int *)((char *)(a3 + i) + 0xc) = local[i];
    }

    _ZN4CP1527FlushAndInvalidateDataCacheEjj(a0, a2);
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(a1, a2);
}

typedef struct Ctx {
    int kind;
    void (*fn)(int, int, int, int, int);
    int arg;
} Ctx;

extern void func_020499bc(int a, int b, int c, Ctx *ctx);
extern void func_020497a0(int a, int b, int c, Ctx *ctx);

void func_02049ba8(int a1, int a2, int a3, int a4, Ctx *ctx) {
    if (ctx->fn)
        ctx->fn(a1, a2, a3, a4, ctx->arg);
    switch (ctx->kind) {
    case 0:
        return;
    case 1:
        func_020499bc(a1, a2, a3, ctx);
        return;
    case 2:
        func_020497a0(a1, a2, a3, ctx);
        return;
    }
}
