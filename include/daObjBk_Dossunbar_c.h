#ifndef DAOBJBK_DOSSUNBAR_C_H
#define DAOBJBK_DOSSUNBAR_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct Player;

/**
 * Whomp's Fortress sliding bars (`dossunbar`). One class, two
 * registry profiles: BK_DOSSUNBAR_L (MOVING_BAR_BIG, actor 0x35)
 * and BK_DOSSUNBAR_S (MOVING_BAR_SMALL, actor 0x36). Factory size
 * 0x338. Direct base is dBgActor_c.
 *
 * `daObjBk_Dossunbar_c` is the RTTI name.
 */
struct daObjBk_Dossunbar_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mHomePosX;     /* 0x320 -- Init copies mPos here */
    s32 mHomePosY;     /* 0x324 */
    s32 mHomePosZ;     /* 0x328 */
    s32 mVariant;      /* 0x32c -- 1 if actor 0x35 (big), else 0 */
    u32 mState;        /* 0x330 -- 0..6, indexes the .bss dispatch table */
    s32 mStateTimer;   /* 0x334 */

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnHitByMegaChar(Player &player);
    virtual void Kill();

    void func_ov015_02111fb8(int idx);                /* install a state */

    /* The 14 state handlers; the two .bss dispatch tables hold PMFs to
       these. */
    void func_ov015_02111ce0();                       /* state 6 body */
    void func_ov015_02111d28();                       /* state 6 enter */
    void func_ov015_02111d4c();                       /* state 5 body */
    void func_ov015_02111d8c();                       /* state 5 enter */
    void func_ov015_02111d98();                       /* state 4 body */
    void func_ov015_02111dd4();                       /* state 4 enter */
    void func_ov015_02111df4();                       /* state 3 body */
    void func_ov015_02111e60();                       /* state 3 enter */
    void func_ov015_02111e80();                       /* state 2 body */
    void func_ov015_02111ee0();                       /* state 2 enter */
    void func_ov015_02111eec();                       /* state 1 body */
    void func_ov015_02111f4c();                       /* state 1 enter */
    void func_ov015_02111f6c();                       /* state 0 body */
    void func_ov015_02111fac();                       /* state 0 enter */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjBk_Dossunbar_c() {}
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBk_Dossunbar_c_size_must_be_0x338[
    sizeof(daObjBk_Dossunbar_c) == 0x338 ? 1 : -1];
#endif

#endif /* DAOBJBK_DOSSUNBAR_C_H */
