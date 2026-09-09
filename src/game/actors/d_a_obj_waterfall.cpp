//cpp
/**
 * Waterfall mist.
 *
 * No model. InitResources succeeds with nothing to load; it only
 * picks a particle ID from the current course (data_0209f2f8): 0x71
 * on 0x16, 0xeb on 0x21, otherwise 0x24. Behavior (re)issues that
 * particle at this actor's position every frame, feeding last frame's
 * handle back so the emitter is recycled.
 *
 * daObjWaterfall_c_classInit is reconstructed (RTTI daObjWaterfall_c,
 * WATERFALL registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: Particle::System::New stays the mangled extern "C". The
 *   ROM name carries Fix12<int> by-value; declaring those types
 *   homes the args to the stack (notes/mwccarm-codegen.md 6az) and
 *   DIFFs the call. Scalar ints mangle to a different symbol.
 *   Do not put a typed extern "C" of the mangled name on
 *   Particle__System.h.
 * Leftover: in-class operator new takes unsigned long and forwards to
 *   _ZN7fBase_cnwEj (unsigned int). Keep `return new daObjWaterfall_c`.
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 * Leftover: data_0209f2f8 is still the tree-wide course/sublevel id.
 * Leftover: three scalar mPosX/Y/Z into New. That is the call, not a
 *   Vector3 pun. dActor_c::Pos() is not on this branch (#2513).
 * Leftover: InitResources is `if (v != 0x16) { if (v == 0x21) } else`.
 *   `if / else if` is a size DIFF (999).
 */

#include "daObjWaterfall_c.h"

extern "C" signed char data_0209f2f8;

/* True signature is Fix12 by-value; see leftover. */
extern "C" u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned handle, unsigned effectID, int x, int y, int z,
    const void *dir, void *cb);

enum {
    kWaterfallMistDefault  = 0x24,
    kWaterfallMistCourse16 = 0x71,
    kWaterfallMistCourse21 = 0xeb
};

// @symbol daObjWaterfall_c_classInit
extern "C" daObjWaterfall_c *daObjWaterfall_c_classInit()
{
    return new daObjWaterfall_c();
}

struct WaterfallSpawnInfo {
    daObjWaterfall_c *(*classInit)();
    s16 executePriority; /* +4 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char WaterfallSpawnInfo_size_must_be_0x1c[
    sizeof(WaterfallSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_WATERFALL
extern "C" WaterfallSpawnInfo g_profile_WATERFALL = {
    daObjWaterfall_c_classInit,
    0x00c5,
    0x0015,
    0x00800003,
    0x00060000,
    0x00300000,
    0x02000000,
    0
};

// @symbol _ZN16daObjWaterfall_c13InitResourcesEv
s32 daObjWaterfall_c::InitResources()
{
    signed char v;
    mParticleID = kWaterfallMistDefault;
    v = data_0209f2f8;
    if (v != 0x16) {
        if (v == 0x21)
            mParticleID = kWaterfallMistCourse21;
    } else {
        mParticleID = kWaterfallMistCourse16;
    }
    return 1;
}

// @symbol _ZN16daObjWaterfall_c8BehaviorEv
s32 daObjWaterfall_c::Behavior()
{
    mParticleHandle = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle, mParticleID, mPosX, mPosY, mPosZ, 0, 0);
    return 1;
}
