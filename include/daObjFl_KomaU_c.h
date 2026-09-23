#ifndef DAOBJFL_KOMAU_C_H
#define DAOBJFL_KOMAU_C_H

#include "types.h"

/* The rotating fire bar. */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "dCcPos_c.h"

struct daObjFl_KomaU_c : dBgActor_c {
    /* dBgActor_c ends at 0x31e; these ids are 4-byte aligned. */
    u8 pad_31e[2];
    u32 mParticleID[2][8]; /* 0x320, 0x340 */
    dCcPos_c mColliders[8]; /* 0x360 */

    virtual ~daObjFl_KomaU_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjFl_KomaU_c_size_must_be_0x540[sizeof(daObjFl_KomaU_c) == 0x540 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJFL_KOMAU_C_H */
