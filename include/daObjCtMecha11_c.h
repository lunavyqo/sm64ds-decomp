#ifndef DAOBJCTMECHA11_C_H
#define DAOBJCTMECHA11_C_H

#include "dBgActor_c.h"
#include "ShadowModel.h"

/* Tick Tock Clock's spinning platform (profile CT_MECHA11). It rolls on X
 * at a step taken from the clock-setting table; setting 2 picks a sign and
 * holds still for the first five frames of each burst. A cuboid shadow sits
 * on the floor under it and scales with the tilt.
 *
 * Cartridge RTTI is daObjCtMecha11_c, base dBgActor_c at offset zero. The
 * factory allocates 0x380. The first member reuses dBgActor_c's tail padding
 * at 0x31e. */
struct daObjCtMecha11_c : dBgActor_c {
    s8  mRandDirection;       /* 0x31e +1 or -1 while the clock setting is random */
    u8  pad_31f;              /* 0x31f */
    u16 mRandTimer;           /* 0x320 frames left in this burst */
    u16 mRandFrames;          /* 0x322 the burst length just rolled */
    s32 mFloorPosY;           /* 0x324 floor the spawn probe hit, else the probe Y */
    ShadowModel mShadowModel; /* 0x328 */
    Matrix4x3 mShadowMat;     /* 0x350 shadow on the floor, yaw only */

    /* Out of line, and the first virtual, so this class's TU emits D1 then D0. */
    virtual ~daObjCtMecha11_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();

    int UpdateShadow();
    void UpdateModel();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjCtMecha11_c_size_must_be_0x380[
    sizeof(daObjCtMecha11_c) == 0x380 ? 1 : -1];
#endif

#endif
