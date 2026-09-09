//cpp
/**
 * Popping lava bubbles.
 *
 * No model. InitResources succeeds with nothing to load. Behavior
 * (re)issues particle 0xb7 at the closest player's position every
 * frame, feeding last frame's handle back so the emitter is recycled.
 *
 * daObjLava_c_classInit is reconstructed (RTTI daObjLava_c, LAVA
 * registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: use the shared header. Position is dActor_c::Pos(), not a
 *   TU-local `(Vector3 *)&mPosX`. Same overlay, one home. Typed
 *   extern "C" of mangled names on dActor_c.h is still poison; an
 *   overlay accessor is not. Convert other `&mPosX` puns to Pos()
 *   the same way, one enrolled file at a time.
 * Leftover: particle call is still func_02022c3c (wrapper around
 *   Particle::System::New that plants the default callback).
 * Leftover: in-class operator new takes unsigned long and forwards to
 *   _ZN7fBase_cnwEj (unsigned int). Keep `return new daObjLava_c`.
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 */

#include "daObjLava_c.h"
#include "Player.h"

enum {
    kLavaBubbleEffect = 0xb7
};

extern "C" u32 func_02022c3c(u32 handle, u32 effectID,
                             Fix12i x, Fix12i y, Fix12i z, const void *dir);

// @symbol daObjLava_c_classInit
extern "C" daObjLava_c *daObjLava_c_classInit()
{
    return new daObjLava_c();
}

struct LavaSpawnInfo {
    daObjLava_c *(*classInit)();
    s16 executePriority; /* +4 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char LavaSpawnInfo_size_must_be_0x1c[sizeof(LavaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_LAVA
extern "C" LavaSpawnInfo g_profile_LAVA = {
    daObjLava_c_classInit,
    0x00c4,
    0x0014,
    0,
    0,
    0x1000,
    0,
    0
};

// @symbol _ZN11daObjLava_c13InitResourcesEv
s32 daObjLava_c::InitResources()
{
    return 1;
}

// @symbol _ZN11daObjLava_c8BehaviorEv
s32 daObjLava_c::Behavior()
{
    Player *player = ClosestPlayer();
    const Vector3 &pos = player->Pos();
    mEffectHandle = func_02022c3c(mEffectHandle, kLavaBubbleEffect,
        pos.x, pos.y, pos.z, 0);
    return 1;
}
