/* The big ice block that moves along a path. */
#ifndef DAOBJEWMICEBLOCK_C_H
#define DAOBJEWMICEBLOCK_C_H

#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct daObjEwmIceBlock_c : dBgActor_c {
    PathPtr mPath;          /* 0x320 */
    s32 mPathNodeIdx;       /* 0x328 */
    s32 mPathDir;           /* 0x32c */

    virtual ~daObjEwmIceBlock_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjEwmIceBlock_c_size_must_be_0x330[
    sizeof(daObjEwmIceBlock_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJEWMICEBLOCK_C_H */
