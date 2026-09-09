//cpp
/**
 * Bowser in the Fire Sea's floating floor.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjUkiyuka_c's shared ov002 helpers.
 * Init also stores BOB_AMPLITUDE into mBobAmplitude.
 *
 * daObjKm2_Ukishima_c_classInit is reconstructed (RTTI
 * daObjKm2_Ukishima_c, KM2_UKISHIMA registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6584 / func_ov002_020b6424 are still the
 *   shared floating-floor setup/teardown; names are placeholders.
 * Leftover: data_ov045_02112f08 is an opaque 3-word file table.
 *   Nothing in this TU dereferences it.
 */

#include "daObjKm2_Ukishima_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b6584(daObjKm2_Ukishima_c *self, ResourceDescriptor *descriptor,
                        Fix12i bobAmplitude);
int func_ov002_020b6424(daObjKm2_Ukishima_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov045_02112f08;
}

struct UkishimaSpawnInfo {
    daObjKm2_Ukishima_c *(*classInit)();
    s16 executePriority; /* +4 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char UkishimaSpawnInfo_size_must_be_0x1c[
    sizeof(UkishimaSpawnInfo) == 0x1c ? 1 : -1];

/* 20.12; the shared setup helper writes this to mBobAmplitude.
   Kept out of the class body: check_header_offsets treats an in-class
   enum as an unparsed field. */
enum { BOB_AMPLITUDE = 0xf50 };

// @symbol daObjKm2_Ukishima_c_classInit
extern "C" daObjKm2_Ukishima_c *daObjKm2_Ukishima_c_classInit()
{
    return new daObjKm2_Ukishima_c();
}

// @symbol g_profile_KM2_UKISHIMA
extern "C" UkishimaSpawnInfo g_profile_KM2_UKISHIMA = {
    daObjKm2_Ukishima_c_classInit,
    0x0091,
    0x00d8,
    2,
    0x00060000,
    0x01000000,
    0x02000000,
    0
};

// @symbol _ZN19daObjKm2_Ukishima_c13InitResourcesEv
s32 daObjKm2_Ukishima_c::InitResources()
{
    return func_ov002_020b6584(this, &data_ov045_02112f08, BOB_AMPLITUDE);
}

// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
s32 daObjKm2_Ukishima_c::CleanupResources()
{
    return func_ov002_020b6424(this, &data_ov045_02112f08);
}
