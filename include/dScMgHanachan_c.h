#ifndef DSCMGHANACHAN_C_H
#define DSCMGHANACHAN_C_H
#include "dScMgBase_c.h"

/* Which Wiggler? Up to fifteen wigglers crawl on the touch screen and one
 * of them has the star. mTimeLeft is the seconds remaining; a wrong touch
 * takes ten off. The destructor, in its own file, cleans up the fifteen
 * 0x98-byte wigglers at 0x4678.
 */
extern "C" void __cxa_vec_cleanup(void *arr, int count, int elemSize, void *dtor);
extern "C" void *func_ov006_020ea324(void);

struct dScMgHanachan_c : dScMgBase_c {
    virtual ~dScMgHanachan_c();
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnYoshiTryEat(int arg);

    /* Scene phase, an Itanium member pointer: Behavior calls it, Render
       compares the pair with the phase tables. */
    u32 mPhaseFn;        /* 0x4660 */
    u32 mPhaseAdj;       /* 0x4664 */
    s32 mDelay;          /* 0x4668 frames before the prompt */
    s32 mResultTimer;    /* 0x466c counts down after a touch */
    s32 mTimeLeft;       /* 0x4670 seconds left */
    s32 mTimeTick;       /* 0x4674 counts down one second */
    u8  mWiggler[0xf][0x98]; /* 0x4678 */
    void *mTarget;       /* 0x4f60 the starred wiggler */
    u8  mHit;            /* 0x4f64 the star was touched */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgHanachan_c_size_must_be_0x4f68[sizeof(dScMgHanachan_c) == 0x4f68 ? 1 : -1];
#endif

#endif
