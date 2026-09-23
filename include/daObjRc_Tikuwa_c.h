#ifndef DAOBJRC_TIKUWA_C_H
#define DAOBJRC_TIKUWA_C_H

#include "types.h"

/* daObjRc_Tikuwa_c -- the donut block that falls. */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

struct daObjRc_Tikuwa_c : dBgActor_c {
    u8 pad_31e[0x2];                 /* 0x31e -- dBgActor_c tail */
    dBgCh_Actr mWithMeshClsn;        /* 0x320 */
    s32 mHomePosX;                   /* 0x4dc */
    s32 mHomePosY;                   /* 0x4e0 */
    s32 mHomePosZ;                   /* 0x4e4 */
    u8 mHadClsn;                     /* 0x4e8 */
    u8 mClsnTimer;                   /* 0x4e9 */
    u8 mState;                       /* 0x4ea */

    virtual ~daObjRc_Tikuwa_c();

    virtual s32 InitResources();       /* slot 0 */
    virtual s32 CleanupResources();    /* slot 3 */
    virtual s32 Behavior();            /* slot 6 */
    virtual s32 Render();              /* slot 9 */
    virtual void OnPendingDestroy();   /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjRc_Tikuwa_c_size_must_be_0x4ec[sizeof(daObjRc_Tikuwa_c) == 0x4ec ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJRC_TIKUWA_C_H */
