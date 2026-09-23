#ifndef DABGSNMHED_C_H
#define DABGSNMHED_C_H

#include "dActor_c.h"
#include "Model.h"
#include "TextureSequence.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* The snowman's head. */
struct daBgSnmHed_c : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    Model mModel;                       /* 0x0d4 */
    TextureSequence mTextureSequence;   /* 0x124 */
    dCcAc_c mCylinder;                  /* 0x138 */
    dBgCh_Actr mWithMeshClsn;           /* 0x16c */

    typedef int (daBgSnmHed_c::*StateFunc)();
    StateFunc *mStateFuncs;             /* 0x328 */
    Player *mTalkPlayer;                /* 0x32c */
    s32 mStateValue;                    /* 0x330 */
    u8 mSubstate;                       /* 0x334 */
    u8 mStateTimer;                     /* 0x335 */
    u8 unk_336;                         /* 0x336 */
    u8 mPad337;                         /* 0x337 */

    virtual ~daBgSnmHed_c();             /* slots 16, 17 */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    void UpdateModel();
    int InitState0();
    int State0();
    int InitState1();
    int State1();
    int InitState2();
    int State2();
    int InitState3();
    int State3();
    void CallStateBehavior();
    void CallStateInit();
    void SetState(int state);
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char SnowmanHead_size_must_be_0x338[
    sizeof(daBgSnmHed_c) == 0x338 ? 1 : -1];
#endif

#endif
