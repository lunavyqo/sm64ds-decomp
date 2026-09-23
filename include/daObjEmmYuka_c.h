#ifndef DAOBJEMMYUKA_C_H
#define DAOBJEMMYUKA_C_H

#include "types.h"

/* Tiny-Huge Island square path lift. Actor 175 EMM_YUKA (ov052,
 * SQUARE_PATH_LIFT). The low byte of param1 selects the path; Behavior
 * walks mNodeIndex along it, reversing at an open end.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct daObjEmmYuka_c : dBgActor_c {
    u8 pad_31e[2];
    PathPtr mPath;       /* 0x320 */
    s32 mNodeIndex;      /* 0x328 */
    s32 mPathDir;        /* 0x32c -- +1 or -1 */

    virtual ~daObjEmmYuka_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjEmmYuka_c_size_must_be_0x330[sizeof(daObjEmmYuka_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJEMMYUKA_C_H */
