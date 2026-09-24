#ifndef DASLIDE_BOX_C_H
#define DASLIDE_BOX_C_H

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

/* Jolly Roger Bay's sliding crate (SLIDE_BOX).
 *
 * RTTI: _ZTS at ov016 0x02114c58 is "13daSlide_Box_c". _ZTI at 0x02114c4c is
 * [__si_class_type_info+8, that string, _ZTI10dBgActor_c]. The vtable address
 * point is 0x02114c8c; the word at 0x02114c88 is that same _ZTI. One base,
 * dBgActor_c, and the table differs from the base only at slots 0, 3, 6, 9,
 * 16 and 17.
 *
 * The factory (next TU, daSlide_Box_c_classInit) allocates 0x4f8, constructs
 * dBgActor_c, then the dBgCh_Actr at 0x324. The destructor tears that member
 * down before the base, which is the same layout.
 */
struct daSlide_Box_c : dBgActor_c {
    dActor_c *mShip;                    /* 0x320 -- actor 0x39, the upward ship */
    dBgCh_Actr mWithMeshClsn;           /* 0x324 */
    Vector3 mBasePos;                   /* 0x4e0 -- position where it grounded */
    Fix12i mHorzPos;                    /* 0x4ec -- travel along the ship's tilt */
    u32 mSoundID;                       /* 0x4f0 -- rolling Sound::PlayLong handle */
    u8 mState;                          /* 0x4f4 */
    u8 pad_4f5[0x3];

    /* Out of line in the TU. One definition; mwccarm emits D1 then D0. */
    virtual ~daSlide_Box_c();           /* slots 16, 17 */

    virtual s32 InitResources();        /* slot  0 */
    virtual s32 CleanupResources();     /* slot  3 */
    virtual s32 Behavior();             /* slot  6 */
    virtual s32 Render();               /* slot  9 */

    /* Model matrix from all three angles, then position at 1/8 scale. */
    void UpdateModel();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daSlide_Box_c_size_must_be_0x4f8[
    sizeof(daSlide_Box_c) == 0x4f8 ? 1 : -1];
#endif

#endif /* DASLIDE_BOX_C_H */
