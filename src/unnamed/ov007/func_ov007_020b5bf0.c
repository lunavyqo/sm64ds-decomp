#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "Ov007Sprite.h"
extern short data_02082214[];
extern u8 data_ov007_0210342c[];

/* The 10-byte layout row func_ov007_020aea6c's item points at (func_ov007_020b4c04
 * passes data_ov007_020d76e8 + index * 10). func_ov007_020ae810 seeds the item
 * position from home, func_ov007_020aeb34 slides it by slide. */
typedef struct Ov007ItemLayout {
    u16 kind;   /* 0x0: this file switches on it */
    s16 homeX;  /* 0x2 */
    s16 homeY;  /* 0x4 */
    s16 slideX; /* 0x6 */
    s16 slideY; /* 0x8 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; only the fields this
 * file touches are spelled. */
typedef struct Ov007Item {
    u16 id;                  /* 0x00: aea6c's first argument (func_ov007_020b4c04's index) */
    char pad_02[2];          /* 0x02 */
    Ov007MenuState *state;   /* 0x04: func_ov007_020c9388; ticked by func_ov007_020ae834 */
    char pad_08[8];          /* 0x08 */
    s32 fade;                /* 0x10: Q12; func_ov007_020ae834 eases it in states 5/6 */
    s32 slideOut;            /* 0x14: Q12; func_ov007_020ae834 eases it in states 3/4/7 and
                            * func_ov007_020aeb34 slides the item by its square */
    s32 wiggleTimer;         /* 0x18: func_ov007_020ae834 counts it down */
    s32 wiggleFrames;        /* 0x1c: the wiggle's length; func_ov007_020ae2d0 arms 0x3c */
    char pad_20[4];          /* 0x20 */
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns (names as in that file). */
typedef struct Ov007ItemView {
    Ov007Item *item;               /* 0x00 */
    Ov007Sprite *sprite;           /* 0x04: func_ov007_020c44e4 */
    char pad_08[8];                /* 0x08 */
    struct Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c */
} Ov007ItemView;

/* data_ov007_0210342c: the overlay's manager; only its settings slot here. */
typedef struct Ov007Work {
    char pad_00[0x28];
    /* 0x28: the 0x3c-byte record func_ov007_020b6a84 allocates; its first word
     * points at a record with no layout yet, so `obj` below stays raw */
    struct Ov007Settings *settings;
} Ov007Work;

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

extern int _ZN4cstd3divEii(int, int);
extern unsigned short func_ov007_020c3be0(int t, int a, int b);
extern void func_ov007_020b5f64(void *a);
extern void func_ov007_020c8f98(int a);

void func_ov007_020b5bf0(Ov007ItemView *view) {
    Ov007Item *r7 = view->item;
    int r6 = 0;
    int r5 = r6;
    int r4 = r6;

    if (r7->layout->kind == 0) {
        if (r7->wiggleTimer > 0) {
            int a = r7->wiggleTimer;
            int diff = r7->wiggleFrames - a;
            int sb;
            if (diff <= 0) {
                sb = r6;
            } else if (diff >= r7->wiggleFrames) {
                sb = 0x1000;
            } else {
                sb = _ZN4cstd3divEii(diff << 0xc, r7->wiggleFrames);
            }
            {
                int t1 = (sb * (r7->wiggleFrames * 0x48)) >> 0xc;
                int r = _ZN4cstd3divEii(t1 << 0x10, 0x168);
                int idx;
                short tblval;
                int inv;
                u16 typeField;
                int r2;
                int mulfactor;
                int result;

                r = (u16)r;
                idx = (r >> 4) * 2;
                tblval = data_02082214[idx];
                inv = 0x1000 - sb;
                typeField = r7->id;
                r2 = FX_MUL(tblval, inv);
                mulfactor = (typeField == 0) ? 4 : 8;
                result = (r2 * mulfactor) >> 0xc;
                view->sprite->rotZ = _ZN4cstd3divEii(result << 0x10, 0x168);
            }
        } else {
            view->sprite->rotZ = r6;
        }
    }

    {
        u16 switchVal = r7->layout->kind;
        int var_r0_2;

        switch (switchVal) {
        case 0:
        case 2:
        case 6:
            r5 = 1;
            break;
        case 4:
        {
            s16 v = view->item->state->currentState;
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r5 = 1;
            }
            break;
        }
        case 8:
            r4 = 1;
            break;
        case 7:
        {
            u16 typeField = r7->id;
            char *obj = *(char **)WORK->settings;
            s16 v = r7->state->currentState;

            if ((v == 2 && (typeField == 9 || (typeField == 0xc && *(u8 *)(obj + 9) != 0) ||
                            (typeField == 0xd && *(u8 *)(obj + 9) == 0))) ||
                (typeField == 0xc && *(u8 *)(obj + 9) == 0) ||
                (typeField == 0xd && *(u8 *)(obj + 9) != 0)) {
                Ov007Sprite *m4 = view->sprite;
                m4->texS = m4->width;
            } else if (v != 2 && (typeField == 9 || (typeField == 0xc && *(u8 *)(obj + 9) != 0) ||
                                  (typeField == 0xd && *(u8 *)(obj + 9) == 0))) {
                view->sprite->texS = 0;
            }
            break;
        }
        case 3:
        {
            s16 v = view->item->state->currentState;
            if ((u32)(u16)(s16)(v - 3) <= 1u) {
                r4 = 1;
            } else {
                r6 = 1;
            }
            break;
        }
        default:
            break;
        }

        var_r0_2 = (r4 != 0) ? (0x1000 - r7->slideOut) : r7->fade;

        if (r5 != 0 || r4 != 0) {
            if (var_r0_2 >= 0x1000) {
                view->sprite->alpha = 0x1f000;
            } else if (var_r0_2 <= 0) {
                view->sprite->alpha = 0;
            } else {
                view->sprite->alpha = FX_MUL(var_r0_2, 0x1f000);
            }
        }

        if (r6 != 0) {
            if (var_r0_2 >= 0x1000) {
                view->sprite->color = 0x7fff;
            } else if (var_r0_2 <= 0) {
                view->sprite->color = 0x4210;
            } else {
                view->sprite->color = func_ov007_020c3be0(var_r0_2, 0x4210, 0x7fff);
            }
        }
    }

    if ((u32)(u16)(r7->id + 0xfff2) <= 2u) {
        func_ov007_020b5f64(view);
    }
    func_ov007_020c8f98((int)view->slideTween);
}

#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Tween.h"
extern u8 data_ov007_0210342c[];

/* The 10-byte layout row an item points at (names as in
 * func_ov007_020b5bf0); only the home position is read here. */
typedef struct Ov007ItemLayout {
    u16 kind;  /* 0x0 */
    s16 homeX; /* 0x2 */
    s16 homeY; /* 0x4 */
} Ov007ItemLayout;

/* The 0x2c-byte record func_ov007_020aea6c allocates; names as in
 * func_ov007_020b5bf0 and func_ov007_020beeb0, only the slots this file
 * touches. */
typedef struct Ov007Item {
    u16 id; /* 0x00: func_ov007_020b4c04's index */
    char pad_02[2];
    Ov007MenuState *state; /* 0x04: func_ov007_020c9388(1) */
    s32 posX;              /* 0x08: Q12 */
    s32 posY;              /* 0x0c */
    char pad_10[0x14];
    Ov007ItemLayout *layout; /* 0x24 */
} Ov007Item;

/* The 0x1c record func_ov007_020b4c04 returns (names as in that file): this
 * function slides item from slideFrom to slideTo by slideT. */
typedef struct Ov007ItemView {
    Ov007Item *item; /* 0x00 */
    char pad_04[8];
    s32 slideT;             /* 0x0c: slideTween output, 0..0x1000 */
    Ov007Tween *slideTween; /* 0x10: func_ov007_020c908c, bound to slideT */
    s16 slideFromX;         /* 0x14 */
    s16 slideFromY;         /* 0x16 */
    s16 slideToX;           /* 0x18 */
    s16 slideToY;           /* 0x1a */
} Ov007ItemView;

/* data_ov007_0210342c: the overlay's manager; names as in
 * func_ov007_020afc44, only the slots this file reads. */
typedef struct Ov007Work {
    char pad_00[4];
    Ov007MenuState *states[3]; /* 0x04: func_ov007_020c9388 */
    char pad_10[0x48];
    int selectedSlot; /* 0x58: medium confidence (func_ov007_020afc44) */
    int targetSlot;   /* 0x5c */
} Ov007Work;

/* The extern keeps this file's u8[] spelling; the manager pointer is stored there. */
#define WORK (*(Ov007Work **)data_ov007_0210342c)

/* The parameter keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is a local. */
void func_ov007_020b5f64(char *self) {
    Ov007ItemView *view = (Ov007ItemView *)self;
    Ov007Item *p;
    Ov007Work *g;
    int slot;
    int x;
    int sel;
    int y;
    Ov007Tween *q;
    int w;

    p = view->item;
    slot = p->id - 0xe;
    g = WORK;
    sel = g->states[1]->currentState;
    x = view->slideToX;
    y = view->slideToY;

    switch (sel) {
    case 0:
        x = p->layout->homeX;
        y = p->layout->homeY;
        break;
    case 1:
        y = 0x24;
        break;
    case 3:
    case 6:
        y = 0x58;
        break;
    case 7:
        if (slot == g->selectedSlot || slot == g->targetSlot)
            y = 0x24;
        else
            y = 0x58;
        break;
    case 9:
        if (slot == g->selectedSlot)
            y = 0x24;
        else
            y = 0x58;
        break;
    }

    if (p->state->stateWork == 2)
        p->state->stateWork = 0;

    if (x != view->slideToX || y != view->slideToY) {
        view->slideFromX = (short)(p->posX >> 12);
        view->slideFromY = (short)(p->posY >> 12);
        view->slideToX = x;
        view->slideToY = y;
        q = view->slideTween;
        q->state = 1;
        q->mode = 1;
        q->t = 0;
        q->step = 0xa0;
        q->from = 0;
        *q->out = q->from;
        q->to = 0x1000;
    } else if (view->slideTween->state == 3) {
        if (p->state->stateWork == 1)
            p->state->stateWork = 2;
    }

    if (view->slideTween->state == 3)
        return;

    w = view->slideT;
    p->posX = (int)(((s64)(0x1000 - w) * ((int)view->slideFromX << 12) +
                     (s64)w * ((int)view->slideToX << 12)) >>
                    12);
    p->posY = (int)(((s64)(0x1000 - w) * ((int)view->slideFromY << 12) +
                     (s64)w * ((int)view->slideToY << 12)) >>
                    12);
}
