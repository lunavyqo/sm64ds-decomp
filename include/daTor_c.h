/* daTor_c -- the sand tornado. */
#ifndef DATOR_C_H
#define DATOR_C_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "TextureTransformer.h"

struct daTor_c : dActor_c {
    u8 pad_0d0[0x4];
    dCcAc_c mdCcAc_c;                       /* 0x0d4 */
    dBgCh_Actr mWithMeshClsn;               /* 0x108 */
    ModelAnim mModelAnim;                   /* 0x2c4 */
    TextureTransformer mTextureTransformer; /* 0x328 */
    /* Actor caught by the spin. Behavior stores it after the hit check;
       State2 clears it. */
    dActor_c *mCaughtActor;                 /* 0x33c */
    s32 mHomePosX;                          /* 0x340 */
    s32 mHomePosY;                          /* 0x344 */
    s32 mHomePosZ;                          /* 0x348 */
    /* How far from home a player is still chased. InitResources builds it
       from param1's low byte. */
    s32 mChaseRange;                        /* 0x34c */
    u16 mStateTimer;                        /* 0x350 */
    u16 unk_352;                            /* 0x352 */
    u16 mChaseTimer;                        /* 0x354 */
    s16 mAngleToHome;                       /* 0x356 */
    s16 mAngleToPlayer;                     /* 0x358 */
    /* UpdateSpin indexes the sine table with this and adds 0x200. */
    s16 unk_35a;                            /* 0x35a */
    s32 mState;                             /* 0x35c -- 0 home, 1 chase, 2 wind down */
    u8 mTriggerCount;                       /* 0x360 */
    u8 pad_361[0x3];
    s32 mParticleHandle0;                   /* 0x364 */
    s32 mParticleHandle1;                   /* 0x368 */
    s32 mSoundHandle;                       /* 0x36c */

    virtual ~daTor_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    void UpdateSpin(s32 scale);
    void State2();
    void State1();
    void State0();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daTor_c_size_must_be_0x370[sizeof(daTor_c) == 0x370 ? 1 : -1];
#endif

#endif
