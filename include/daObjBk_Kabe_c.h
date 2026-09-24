#ifndef DAOBJBK_KABE_C_H
#define DAOBJBK_KABE_C_H

#include "types.h"

/* Bowser's castle walls. BK_KABE00 (actor 0x30) breaks, plays the secret
 * jingle and spawns a star; BK_KABE01 (actor 0x2f) is destroyed on Kill.
 * Cartridge RTTI is _ZTS14daObjBk_Kabe_c at ov079 0x02128044, and this
 * class's vtable at 0x021280b0 points at that typeinfo.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daObjBk_Kabe_c : dBgActor_c {
    /* 0 for the breakable wall, 1 otherwise. Indexes the model, KCL and
       CLPS rows in ov079. */
    u8 mVariant;                      /* 0x31e */
    /* Low byte of param1, with 0xff read as star 0. ORed with 0x40 when
       the star (actor 0xb2) is spawned. */
    u8 mStarId;                       /* 0x31f */
    u8 pad_320[0x1];
    /* Set by Kill on the breakable wall. Render skips the model, and
       Behavior runs the break sequence, while this is set. */
    u8 mBroken;                       /* 0x321 */
    /* Sound::PlaySecretSound advances this. The star spawns on the frame
       that call returns nonzero. */
    u16 mBreakSoundState;             /* 0x322 */

    virtual ~daObjBk_Kabe_c();

    virtual int InitResources();                 /* slot  0 */
    virtual int CleanupResources();              /* slot  3 */
    virtual int Behavior();                      /* slot  6 */
    virtual int Render();                        /* slot  9 */
    virtual int OnHitByCannonBlastedChar(dActor_c &other); /* slot 26 */
    virtual void Kill();                         /* slot 31 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBk_Kabe_c_size_must_be_0x324[sizeof(daObjBk_Kabe_c) == 0x324 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJBK_KABE_C_H */
