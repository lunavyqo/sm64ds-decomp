/* daObjHeart_c -- the healing heart pickup. */
#ifndef DAOBJHEART_C_H
#define DAOBJHEART_C_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct daObjHeart_c : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim; /* 0x0d4 */
    dCcAc_c mdCcAc_c;     /* 0x138 */
    /* Eased toward 0x8000 while mHealTimer is above 0x2d, else toward 0x1000,
       then copied into the model animation speed. */
    s32 mAnimSpeed;       /* 0x16c */
    u8 mHealTimer;        /* 0x170 */
    /* Latched the first frame the collider reports an occupant. */
    u8 mWasTouched;       /* 0x171 */

    virtual ~daObjHeart_c();
    virtual s32 InitResources();    /* slot  0 */
    virtual s32 CleanupResources(); /* slot  3 */
    virtual s32 Behavior();         /* slot  6 */
    virtual s32 Render();           /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjHeart_c_size_must_be_0x174[sizeof(daObjHeart_c) == 0x174 ? 1 : -1];
#endif

#endif
