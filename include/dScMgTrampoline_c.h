/* dScMgTrampoline_c — Trampoline Time. Draw up to three lines on the touch
 * screen; Mario bounces off them toward the lit door. Three misses end
 * the round. Child of dScMgD3DBase_c, size 0x5dc8 from the factory's
 * operator new.
 *
 * The destructor is defined in the class body. An out-of-line definition
 * emits D0 before D1, plus a D2 this ROM does not have, so this translation
 * unit cannot be linked in address order. InitResources is the first virtual
 * not defined here and stays the key function. dScMgD3DBase_c supplies
 * operator delete.
 *
 * The three element arrays stay raw: their private types are not recovered.
 * From 0x5d84 the tail is the flashing door mark, the scroll, the stylus
 * sample, and the timer that moves the lit door.
 */
#ifndef DSCMGTRAMPOLINE_C_H
#define DSCMGTRAMPOLINE_C_H
#include "dScMgD3DBase_c.h"

extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void *func_ov006_020ccfc8(void);
extern "C" void func_ov006_020d1008(void);
extern "C" void func_ov006_02120938(void);

struct dScMgTrampoline_c : dScMgD3DBase_c {
    typedef void (dScMgTrampoline_c::*State)();

    /* In the class body so D1 is emitted before D0 and no D2 is emitted.
       A virtual destructor is still reached through the vtable, not inlined
       into callers. */
    virtual ~dScMgTrampoline_c() {
        __cxa_vec_cleanup(mArray3, 5, 0x24, (void *)func_ov006_02120938);
        __cxa_vec_cleanup(mArray2, 3, 0x32c, (void *)func_ov006_020d1008);
        __cxa_vec_cleanup(mArray1, 4, 0xd0, (void *)func_ov006_020ccfc8);
    }

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnYoshiTryEat(int arg);
    virtual int  OnTurnIntoEgg(int mode);
    virtual int  OnAttacked2();
    virtual int  OnKicked();
    virtual int  OnPushed();
    virtual void Virtual88(int cx, int cy, int colour, int size);

    /* State callbacks stored in mState. Behavior dispatches the same
       eight-byte pointer-to-member every frame. */
    void StateDone();
    void StateWaitExit();
    void StateResults();
    void StatePlay();
    void StateIntro();

    void BeginResults();
    void BeginPlay();
    void BeginIntro();

    void UpdateTouchInput();
    void UpdateScroll();

    u32 mState[2];          /* 0x5004 -- raw eight-byte State encoding; a
                               typed global PMF emits a non-ROM __sinit */
    u8  mArray1[0x340];     /* 0x500c -- 4 * 0xd0; StatePlay launches one */
    u8  mArray2[0x984];     /* 0x534c -- 3 * 0x32c, elem dtor func_ov006_020d1008 */
    u8  mArray3[0xb4];      /* 0x5cd0 -- 5 * 0x24 floating scores (Desc) */
    s16 mDoorMarkRow;       /* 0x5d84 -- which door the flash marks, 0 or 1 */
    s16 mDoorMarkTimer;     /* 0x5d86 -- counts down from 0x3c while it blinks */
    s32 mDoorMarkActive;    /* 0x5d88 -- nonzero while that blink is running */
    s32 mDoorMarkShown;     /* 0x5d8c -- toggles with mDoorMarkTimer / 10 */
    int mTimer;             /* 0x5d90 -- countdown gating state transitions */
    s32 mScrollY;           /* 0x5d94 -- approached toward mScrollTargetY by 2 a
                               tick; the BG2 offset is mScrollY + mScrollOffsetY */
    s32 mScrollTargetY;     /* 0x5d98 */
    s32 mScrollHoldTimer;   /* 0x5d9c -- 0x78, run down to 0 before the target
                               may move again */
    s32 mScrollOffsetY;     /* 0x5da0 -- added to mScrollY everywhere it is used */
    s32 mArrow1X;           /* 0x5da4 -- the two guide arrows Render draws at
                               n + 0xf0; each slides between 0 and 0x20 */
    s32 mArrow2X;           /* 0x5da8 */
    s32 mDragSoundHandle;   /* 0x5dac -- returned by the positional drag-sound
                               updater and fed back on the next frame */
    s16 mTouchX;            /* 0x5db0 -- the current stylus sample */
    s16 mTouchY;            /* 0x5db2 */
    s16 mTouchStartX;       /* 0x5db4 -- copied from mTouch on the press edge and
                               left alone; the swipe is measured against it */
    s16 mTouchStartY;       /* 0x5db6 */
    s16 mInputEnabled;      /* 0x5db8 -- while 0 the touch handler clears
                               mTouching and returns at once */
    s16 mDoorSide;          /* 0x5dba -- 0 or 1, the lit door; the arrows slide
                               toward it and the mark copies it */
    s16 mDoorSwitchTimer;   /* 0x5dbc -- counts down to the next door change */
    s16 mDoorBlinkTimer;    /* 0x5dbe -- 0..0xa once the switch is within 0x3c */
    s16 mDoorBlinkPhase;    /* 0x5dc0 -- while set, the lit door's arrow uses
                               the other frame */
    s16 mRoundOver;         /* 0x5dc2 -- set once results clear the screen;
                               Render then skips the playfield */
    u8  mTouching;          /* 0x5dc4 -- a drag is in progress */
    u8  mTouchReleased;     /* 0x5dc5 -- set on the release edge; OnAttacked2
                               consumes and clears it after the swipe test */
    u8  pad_5dc6[0x2];      /* 0x5dc6 -- rounds up to the 0x5dc8 boundary */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgTrampoline_c_size_must_be_0x5dc8[sizeof(dScMgTrampoline_c) == 0x5dc8 ? 1 : -1];
#endif

#endif
