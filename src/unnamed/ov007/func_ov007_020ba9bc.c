extern int func_ov007_020bad8c(void);
extern int func_ov007_020baa10(void);
extern int func_ov007_020c5188(void*, int);
struct G { char* obj; char pad[0x18]; int n; };
extern struct G* data_ov007_02104ba0;
extern char data_ov007_020d7b80[];
void func_ov007_020ba9bc(void){
  func_ov007_020bad8c();
  func_ov007_020baa10();
  *(int*)(data_ov007_02104ba0->obj+0x18) = (int)(data_ov007_020d7b80 + data_ov007_02104ba0->n*0x18);
  func_ov007_020c5188(data_ov007_02104ba0->obj, 0);
}


#include "types.h"
// @symbol func_ov007_020baa10
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
extern s16 data_02082214[];

/* The 0xd0-byte touch picker (func_ov007_020c690c); names as in
 * func_ov007_020c684c, only the slots this file touches. */
typedef struct Ov007Picker {
    char pad_00[0x70];
    Vector3 pos;       /* 0x70 */
    Vector3 restPos;   /* 0x7c: func_ov007_020c6af0 snapshots pos here */
    Vector3 posOffset; /* 0x88: func_ov007_020c5188 adds it to pos for one step */
    char pad_94[0x24];
    Vector3 boundsMin; /* 0xb8: func_ov007_020c510c, the clamp rectangle */
    Vector3 boundsMax; /* 0xc4 */
} Ov007Picker;

/* data_ov007_02104ba0: the picker scene func_ov007_020b9880 allocates. */
typedef struct Ov007PickerScene {
    Ov007Picker *picker;       /* 0x00: func_ov007_020c690c */
    Ov007MenuState *states[4]; /* 0x04: func_ov007_020c9388; [1] is the main one */
} Ov007PickerScene;

/* The extern keeps the tree's char * spelling (the declaration-agreement
 * gate); the typed view is reached through a cast macro. */
#define SCENE ((Ov007PickerScene *)data_ov007_02104ba0)

/* Each menu state drives the picker's per-step offset across the clamp
 * rectangle; state 5 puts pos back at its rest position. */
void func_ov007_020baa10(void)
{
    Ov007Picker *a;
    s16 state;
    int dx;
    int dy;
    int s;
    Ov007MenuState *menu;
    int counter;

    a = SCENE->picker;
    menu = SCENE->states[3];
    dx = a->boundsMax.x - a->boundsMin.x;
    dy = a->boundsMax.y - a->boundsMin.y;
    func_ov007_020c92d0((char *)menu);
    menu = SCENE->states[3];
    state = menu->currentState;
    counter = menu->stateTicks;

    switch (state) {
    case 0:
        if (counter == 0) {
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    case 1:
    case 2: {
        int v;
        int m = _ZN4cstd3modEii(counter, 0x3c);
        if (m <= 0) v = 0;
        else if (m >= 0x3c) v = 0x1000;
        else v = _ZN4cstd3divEii(m << 12, 0x3c);
        s = data_02082214[((u16)((v * 0xffff) >> 12) >> 4) * 2];
        if (state == 2) {
            a->posOffset.x = -dx / 48 + func_ov007_020c3ba8(dx / 48 - -dx / 48);
            a->posOffset.y = FX_MUL(s, (dy / 2 - dy / 8));
        } else if (state == 1) {
            a->posOffset.x = FX_MUL(s, (dx / 3));
            a->posOffset.y = -dy / 32 + func_ov007_020c3ba8(dy / 32 - -dy / 32);
        }
        return;
    }
    case 3:
    case 4: {
        int v;
        int idx;
        int m = _ZN4cstd3modEii(counter, 0x3c);
        if (m <= 0) v = 0;
        else if (m >= 0x3c) v = 0x1000;
        else v = _ZN4cstd3divEii(m << 12, 0x3c);
        if (SCENE->states[3]->currentState == 4)
            v = 0x1000 - v;
        idx = ((u16)((v * 0xffff) >> 12) >> 4) * 2;
        a->posOffset.x = FX_MUL((dx / 4), data_02082214[idx + 1]);
        a->posOffset.y = FX_MUL((dy / 4), data_02082214[idx]);
        return;
    }
    case 5:
        if (counter == 0) {
            a->pos = a->restPos;
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    case 6:
        if (counter == 0) {
            int t;
            t = -dx / 2 + func_ov007_020c3ba8(dx / 2 - -dx / 2);
            a->pos.x = FX_MUL(t, 0x5dcLL);
            t = -dy / 2 + func_ov007_020c3ba8(dy / 2 - -dy / 2);
            a->pos.y = FX_MUL(t, 0x5dcLL);
            a->posOffset.y = 0;
            a->posOffset.x = a->posOffset.y;
        }
        return;
    }
}


#include "types.h"
#include "nitro/fx/fx.h"
extern u8 data_ov007_02104ba0[];
extern u16 data_ov007_020d7b54[];
extern short data_02082214[];

extern void func_ov007_020c92d0(void *a);
extern int _ZN4cstd3modEii(int, int);
extern int _ZN4cstd3divEii(int, int);
extern void func_ov007_020bbc08(int a, int b, int c);
extern void func_ov007_020c55bc(int a, int b, int c);

void func_ov007_020bad8c(void)
{
    char *mgr;
    char *obj;
    int r6;
    int r5;
    u16 period;
    int var_r4;
    int t;
    s16 sw;

    mgr = *(char **)data_ov007_02104ba0;
    r6 = *(int *)mgr;
    func_ov007_020c92d0(*(char **)(mgr + 0xc));

    obj = *(char **)(*(char **)data_ov007_02104ba0 + 0xc);
    r5 = *(int *)(obj + 0xc);
    period = data_ov007_020d7b54[*(s16 *)obj];
    t = _ZN4cstd3modEii(r5, period);
    if (t <= 0) {
        var_r4 = 0;
    } else if (t >= period) {
        var_r4 = 0x1000;
    } else {
        var_r4 = _ZN4cstd3divEii(t << 0xc, period);
    }

    mgr = *(char **)data_ov007_02104ba0;
    obj = *(char **)(mgr + 0xc);
    sw = *(s16 *)obj;

    switch (sw) {
    case 0:
        if (r5 != 0) {
            return;
        }
        func_ov007_020bbc08(1, 0, 0);
        return;
    case 1:
        *(int *)(mgr + 0x1c) = 0;
        if (_ZN4cstd3modEii(r5, 6) != 0) {
            return;
        }
        {
            int v;
            if (var_r4 >= 0x800) {
                v = 0x1000 - ((var_r4 - 0x800) * 2);
            } else {
                v = var_r4 * 2;
            }
            func_ov007_020bbc08(3, 0xa, (int)(((long long)v * (0x2000 - v)) >> 0xc));
        }
        return;
    case 2:
    {
        int v;
        int amt;
        if (var_r4 >= 0x800) {
            v = 0x1000 - ((var_r4 - 0x800) * 2);
        } else {
            v = var_r4 * 2;
        }
        amt = (int)(((long long)v * (0x2000 - v)) >> 0xc) + 0x1000;
        func_ov007_020c55bc(r6, amt, amt);
        obj = *(char **)(*(char **)data_ov007_02104ba0 + 0xc);
        if (r5 >= data_ov007_020d7b54[*(s16 *)obj]) {
            *(s16 *)(obj + 2) = 0;
        }
        if (var_r4 <= 0x800) {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 4;
        } else {
            *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 0;
        }
        return;
    }
    case 3:
    {
        int d;
        u16 p3;
        int m;
        int var_r0;

        d = r5 - 0x14;
        if (d < 0) {
            return;
        }
        if (d > data_ov007_020d7b54[sw] * 3) {
            return;
        }
        p3 = data_ov007_020d7b54[3];
        m = _ZN4cstd3modEii(d, p3);
        if (m <= 0) {
            var_r0 = 0;
        } else if (m >= p3) {
            var_r0 = 0x1000;
        } else {
            var_r0 = _ZN4cstd3divEii(m << 0xc, p3);
        }
        {
            u32 u = (u32)(var_r0 * 0xffff);
            int idx;
            short tblval;
            int amt;
            u = u << 4;
            u = u >> 0x10;
            idx = (int)u >> 4;
            tblval = data_02082214[idx * 2];
            amt = FX_MUL(tblval, 0x258);
            func_ov007_020c55bc(r6, amt + 0x1000, 0x1000 - amt);
        }
        *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        return;
    }
    case 4:
    {
        int m = _ZN4cstd3modEii(r5, 0x3c);
        *(int *)(*(char **)data_ov007_02104ba0 + 0x1c) = 2;
        if (m == 0) {
            func_ov007_020bbc08(1, 0, 0);
            return;
        }
        if (m != 0x1e) {
            return;
        }
        func_ov007_020bbc08(3, 0x1e, FX_MUL(var_r4, 0x7d0) + 0xc8);
        return;
    }
    case 5:
    default:
        return;
    }
}
