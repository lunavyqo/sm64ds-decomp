#include "types.h"
extern void func_ov007_020c86c4(void* p);
extern void func_ov007_020c897c(void* p);
extern void func_ov007_020c8970(void* p);
extern void func_ov007_020c8688(void* p, int a);
extern void func_ov007_020c86a8(void* p, int a);
extern void func_ov007_020c8c88(void* p, int a);
extern void func_ov007_020c8bc8(void* p, int a);
extern void func_ov007_020c8b34(void* p, int a);

typedef void (*VFN)(void);

/* Reconstructed layout and name, not recovered class identity: the screen
 * transition record in data_ov007_02104bd8 slot 0 (func_ov007_020bf690 drives
 * it, func_ov007_020bf790 configures it). Only the slots this file reads; the
 * effect parameters from +0x28 on are read by the per-effect appliers. */
typedef struct Ov007Transition {
    s32 effect;    /* 0x00: picks the applier below: 1 func_ov007_020c8c88 (mosaic),
                    * 2/3 func_ov007_020c8bc8 (blend alpha), 4 func_ov007_020c8b34
                    * (brightness); func_ov007_020c86c4/020c897c switch on it too */
    u32 screens;   /* 0x04: bit 0 main engine (0x0400xxxx), bit 1 sub (0x04001xxx) */
    s32 state;     /* 0x08: 0 idle (func_ov007_020c897c), 1 running
                    * (func_ov007_020c86c4), 3 held at the end (endAction 1) */
    s32 endAction; /* 0x0c: on reaching t = 0x1000: 0 stop, 1 hold,
                    * 2/3 restart forwards/backwards by `reversed` */
    s32 curve;     /* 0x10: 0 linear, 1 out-and-back, 2 squared, 3 none */
    s32 t;         /* 0x14: Q12 progress, clamped at 0x1000 */
    s32 step;      /* 0x18: added to t each frame (func_ov007_020bf790 seeds it) */
    s32 reversed;  /* 0x1c: 1 runs the curve backwards (func_ov007_020c8688 sets 1,
                    * func_ov007_020c86a8 sets 0) */
    s32 request;   /* 0x20: 1 start (func_ov007_020c86c4), 2 stop (func_ov007_020c897c);
                    * both clear it */
    VFN onEnd;     /* 0x24: called when t reaches the end; func_ov007_020bf790 seeds it */
} Ov007Transition;

/* r5 keeps the char * spelling of the extern; `tr` is the typed view. */
void func_ov007_020c8498(char* r5)
{
    Ov007Transition *tr = (Ov007Transition *)r5;
    int r4 = 0;
    VFN r6;

    if (tr->request == 1) {
        func_ov007_020c86c4(r5);
    } else if (tr->request == 2) {
        func_ov007_020c897c(r5);
    }

    if (tr->state == 0)
        return;

    if (tr->state == 1) {
        int *v14 = &tr->t;
        *v14 += tr->step;
        if (tr->t > 0x1000) {
            tr->t = 0x1000;
            switch (tr->endAction) {
            case 0:
                func_ov007_020c8970(r5);
                break;
            case 1:
                tr->state = 3;
                break;
            case 2:
                if (tr->reversed == 1)
                    func_ov007_020c8688(r5, tr->screens);
                else
                    func_ov007_020c86a8(r5, tr->screens);
                break;
            case 3:
                if (tr->reversed == 1)
                    func_ov007_020c86a8(r5, tr->screens);
                else
                    func_ov007_020c8688(r5, tr->screens);
                break;
            }

            r6 = tr->onEnd;
            if(r6 != 0){
                r6();
            }
            if (r6 == tr->onEnd && (u32)tr->endAction <= 1)
                tr->onEnd = 0;
        }
    }

    if (tr->state == 0)
        return;

    switch (tr->curve) {
    case 0:
        r4 = tr->t;
        break;
    case 1:
        r4 = tr->t << 1;
        if(r4 > 0x1000) r4 = -(r4 - 0x2000);
        break;
    case 2:
        r4 = (int)(((long long)tr->t * tr->t) >> 12);
        break;
    case 3:
        break;
    }

    if (tr->reversed == 1)
        r4 = 0x1000 - r4;

    switch (tr->effect) {
    case 1:
        func_ov007_020c8c88(r5, r4);
        break;
    case 2:
    case 3:
        func_ov007_020c8bc8(r5, r4);
        break;
    case 4:
        func_ov007_020c8b34(r5, r4);
        break;
    default:
        break;
    }
}


typedef struct {
    int _0;
    int _4;
    int _8;
    int _c;
    int _10;
    int _14;
    int _18;
    int _1c;
} S;

extern void func_ov007_020c86c4(S*, int);

void func_ov007_020c8688(S* p, int a) {
    p->_1c = 1;
    p->_4 = a;
    p->_14 = 0;
    func_ov007_020c86c4(p, 0);
}


extern void func_ov007_020c86c4(void *c);
/* c: no class or header names this record (callers 020bf790, 020c8498), so raw. */
void func_ov007_020c86a8(void *c, int v) {
    *(int *)((char *)c + 0x1c) = 0;
    *(int *)((char *)c + 4) = v;
    *(int *)((char *)c + 0x14) = 0;
    func_ov007_020c86c4(c);
}


#include "types.h"
typedef struct S {
    s32 mode;
    u32 flags;
    s32 a;
    s32 pad0c;
    s32 pad10;
    s32 b;
    s32 pad18;
    s32 cond;
    s32 c;
    s32 pad24;
    s32 f28;
    s32 f2c;
    union {
        s32 f30;
        unsigned char byte[4];
    };
} S;

extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile void *p, int a, int b, int c, int d);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, int a, int b);

void func_ov007_020c86c4(S *p)
{
    p->a = 1;
    p->b = 0;
    p->c = 0;

    switch (p->mode) {
    case 0:
        break;

    case 1: {
        s32 mask = p->f28;
        if (p->flags & 1) {
            if (mask & 1) *(volatile u16 *)0x4000008 |= 0x40; else *(volatile u16 *)0x4000008 &= ~0x40;
            if (mask & 2) *(volatile u16 *)0x400000a |= 0x40; else *(volatile u16 *)0x400000a &= ~0x40;
            if (mask & 4) *(volatile u16 *)0x400000c |= 0x40; else *(volatile u16 *)0x400000c &= ~0x40;
            if (mask & 8) *(volatile u16 *)0x400000e |= 0x40; else *(volatile u16 *)0x400000e &= ~0x40;
        }
        if (!(p->flags & 2))
            return;
        if (mask & 1) *(volatile u16 *)0x4001008 |= 0x40; else *(volatile u16 *)0x4001008 &= ~0x40;
        if (mask & 2) *(volatile u16 *)0x400100a |= 0x40; else *(volatile u16 *)0x400100a &= ~0x40;
        if (mask & 4) *(volatile u16 *)0x400100c |= 0x40; else *(volatile u16 *)0x400100c &= ~0x40;
        if (mask & 8) *(volatile u16 *)0x400100e |= 0x40; else *(volatile u16 *)0x400100e &= ~0x40;
        return;
    }

    case 2:
    case 3:
        if (p->flags & 1) {
            _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4000050, p->f28, p->f2c,
                p->cond ? p->byte[2] : p->byte[0],
                p->cond ? p->byte[3] : p->byte[1]);
        }
        if (!(p->flags & 2))
            return;
        _ZN3G2x13SetBlendAlphaEPVttttj((volatile void *)0x4001050, p->f28, p->f2c,
            p->cond ? p->byte[2] : p->byte[0],
            p->cond ? p->byte[3] : p->byte[1]);
        return;

    case 4:
        if (p->flags & 1) {
            _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4000050, p->f28,
                p->cond ? p->f30 : p->f2c);
        }
        if (!(p->flags & 2))
            return;
        _ZN3G2x18SetBlendBrightnessEPVtts((void *)0x4001050, p->f28,
            p->cond ? p->f30 : p->f2c);
        return;
    }
}


// Long-branch veneer. #pragma long_calls makes mwccarm emit the pooled
// `ldr ip,[pc]; bx ip` absolute tail-call the ROM uses to reach the target.
#pragma long_calls on
extern int func_ov007_020c897c(void);

int func_ov007_020c8970(void)
{
    return func_ov007_020c897c();
}


#include "types.h"
struct S {
    s32 mode;
    u32 flags;
    s32 a;
    s32 pad[2];
    s32 b;
    s32 pad2[2];
    s32 c;
};

void func_ov007_020c897c(struct S *p) {
    p->a = 0;
    p->b = 0x1000;
    p->c = 0;
    switch (p->mode) {
    case 1:
        if (p->flags & 1) {
            *(volatile u16 *)0x4000008 &= ~0x40;
            *(volatile u16 *)0x400000a &= ~0x40;
            *(volatile u16 *)0x400000c &= ~0x40;
            *(volatile u16 *)0x400000e &= ~0x40;
        }
        if (p->flags & 2) {
            *(volatile u16 *)0x4001008 &= ~0x40;
            *(volatile u16 *)0x400100a &= ~0x40;
            *(volatile u16 *)0x400100c &= ~0x40;
            *(volatile u16 *)0x400100e &= ~0x40;
        }
        break;
    case 2:
    case 3:
    case 4:
        if (p->flags & 1) {
            *(volatile u16 *)0x4000050 = 0;
        }
        if (p->flags & 2) {
            *(volatile u16 *)0x4001050 = 0;
        }
        break;
    }
}
