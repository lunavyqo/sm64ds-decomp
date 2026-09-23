#ifndef DAOBJBKKILLERDAI_C_H
#define DAOBJBKKILLERDAI_C_H

#include "types.h"
#include "dBgActor_c.h"

struct Player;

/* Bullet Bill launcher. The tail word is the unique ID of the bill it
 * last fired: InitResources clears it, Behavior hands it to FindWithID. */
struct daObjBkKillerdai_c : dBgActor_c {
    u32 mSpawnedUniqueID; /* 0x320 */

    virtual ~daObjBkKillerdai_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnHitByMegaChar(Player &player);
    virtual void Kill();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBkKillerdai_c_size_must_be_0x324[sizeof(daObjBkKillerdai_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJBKKILLERDAI_C_H */
