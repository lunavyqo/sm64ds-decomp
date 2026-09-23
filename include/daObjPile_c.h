#ifndef DAOBJPILE_C_H
#define DAOBJPILE_C_H

#include "dBgActor_c.h"

/* The stump. A ground pound sinks it one step; a mega hit sinks the rest.
 * The last step drops coins. */
struct daObjPile_c : dBgActor_c {
    u8  mState;              /* 0x31e */
    u8  mAttackCooldown;     /* 0x31f */
    u8  mBusy;               /* 0x320 */
    u8  pad_321[3];
    s32 mAngleDelta;         /* 0x324 */
    s16 mPrevAngle;          /* 0x328 */
    u8  pad_32a[2];
    dActor_c *mLinkedTag;    /* 0x32c */

    virtual ~daObjPile_c();

    virtual s32 CleanupResources();
    virtual s32 Render();
    virtual s32 InitResources();
    virtual s32 Behavior();
    void OnGroundPounded(dActor_c &other);
    void OnHitByMegaChar(Player &player);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjPile_c_size_must_be_0x330[sizeof(daObjPile_c) == 0x330 ? 1 : -1];
#endif

#endif
