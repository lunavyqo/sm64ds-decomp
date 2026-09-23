#ifndef DAOBJCTROTATEBLOCK_C_H
#define DAOBJCTROTATEBLOCK_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* The rotating cube on Tick Tock Clock. */
struct daObjCtRotateBlock_c : dBgActor_c {
    Model mRotatingModel;      /* 0x320 the part that turns */
    s32 mOffsetY;              /* 0x370 hop added to mPosY */
    u16 mWaitTimer;            /* 0x374 DecIfAbove0_Short countdown */
    u8 mState;                 /* 0x376 0 wait, 1 hop, 2 turn */
    u8 mVariant;               /* 0x377 actor 0x6c or 0x6d */
    s16 mTargetAngleZ;         /* 0x378 ApproachLinear target */
    u8 mUnevenGround;          /* 0x37a the two floor probes disagreed */
    u8 mPad37b;                /* 0x37b */
    s32 mFloorY;               /* 0x37c ground height under the cube */
    ShadowModel mShadowModel;  /* 0x380 */
    Matrix4x3 mShadowMat;      /* 0x3a8 */

    /* Defined out of line, first in the TU. */
    virtual ~daObjCtRotateBlock_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();

    s32 UpdateShadow();
    void UpdateClsn();
    void UpdateModel();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjCtRotateBlock_c_size_must_be_0x3d8[
    sizeof(daObjCtRotateBlock_c) == 0x3d8 ? 1 : -1];
#endif

#endif
