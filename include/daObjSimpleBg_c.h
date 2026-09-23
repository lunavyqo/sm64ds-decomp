#ifndef DAOBJSIMPLEBG_C_H
#define DAOBJSIMPLEBG_C_H

#include "types.h"
#include "dBgActor_c.h"

/* Static scenery for KI_IWA, KI_HASIRA_DAI, BK_TOWER, DOKAN, MC_DODAI
 * and MC_HAZAD. The cartridge spells the class daObjSimpleBg_c. actorID
 * picks a row of this overlay's model, collision and CLPS tables and
 * stores it in mVariant. The six classInit factories are the next TU.
 *
 * dBgActor_c ends at 0x31e and its size rounds to 0x320, so mVariant
 * sits in that tail byte and sizeof stays 0x320.
 */

struct daObjSimpleBg_c : dBgActor_c {
    u8 mVariant; /* 0x31e */

    virtual ~daObjSimpleBg_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daObjSimpleBg_c_size_must_be_0x320[sizeof(daObjSimpleBg_c) == 0x320 ? 1 : -1];
#endif

#endif /* DAOBJSIMPLEBG_C_H */
