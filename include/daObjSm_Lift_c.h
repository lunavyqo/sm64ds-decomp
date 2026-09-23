#ifndef DAOBJSM_LIFT_C_H
#define DAOBJSM_LIFT_C_H

#include "types.h"
#include "dBgActor_c.h"

/* Cool, Cool Mountain's ski lift. It rides a path and yaws when a
 * mega mushroom hits it. Own fields start where dBgActor_c ends. */

struct daObjSm_Lift_c : dBgActor_c {
    dActor_c *unk_320;          /* 0x320 */
    s32 mBasePosX;              /* 0x324 */
    s32 mBasePosY;              /* 0x328 */
    s32 mBasePosZ;              /* 0x32c */
    u8 unk_330;                 /* 0x330 */
    u8 unk_331;                 /* 0x331 */
    u8 pad_332[2];              /* 0x332 */
    s32 mPathId;                /* 0x334 */
    s32 mNodeCount;             /* 0x338 */
    s32 mNodeIndex;             /* 0x33c */
    u8 pad_340[0xc];            /* 0x340 */
    s32 mNodeStep;              /* 0x34c */
    u32 unk_350;                /* 0x350 */

    virtual ~daObjSm_Lift_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnHitByMegaChar(Player &player); /* slot 27 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjSm_Lift_c_size_must_be_0x354[sizeof(daObjSm_Lift_c) == 0x354 ? 1 : -1];
#endif

#endif /* DAOBJSM_LIFT_C_H */
