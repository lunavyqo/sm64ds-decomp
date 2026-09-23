/**
 * Race flag. Mario touching mdCcAc_c ends the course timer.
 */
#ifndef DARFLAG_C_H
#define DARFLAG_C_H
#include "dActor_c.h"
#include "ModelAnim.h"
#include "dCcAc_c.h"

struct daRFlag_c : dActor_c {
    u8 pad_0d0[0x4];
    dCcAc_c mdCcAc_c;       /* 0x0d4 touch cylinder */
    ModelAnim mModelAnim;   /* 0x108 */
    u16 mVictoryTimer;      /* 0x16c frames since Mario touched the flag */
    u8 mHasTouchedFlag;     /* 0x16e 0 until Mario touches it, then 1; init writes 0xff */
    u8 pad_16f[0x5];        /* nothing in this class reads these */
    virtual ~daRFlag_c();

    virtual int InitResources();
    virtual int CleanupResources();
    virtual int Behavior();
    virtual int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daRFlag_c_size_must_be_0x174[sizeof(struct daRFlag_c) == 0x174 ? 1 : -1];
#endif

#endif
