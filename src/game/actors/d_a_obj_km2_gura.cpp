//cpp
/**
 * Bowser in the Fire Sea's tilting slab.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjGuragura_c's shared ov002 helpers.
 *
 * The three-word file table is defined in this TU (retail places it
 * between the typeinfo and the type name). Keep that definition after
 * the class header.
 *
 * daObjKm2_Gura_c_classInit is reconstructed (RTTI daObjKm2_Gura_c,
 * KM2_GURA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b6244 / func_ov002_020b60fc are still the
 *   shared tilting-slab setup/teardown; names are placeholders.
 * Leftover: the file table's three pointers are still data_ov045_*.
 */

#include "daObjKm2_Gura_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
extern char data_ov045_02113220[];
extern char data_ov045_02113228[];
extern char data_ov045_021124f0[];
}

extern "C" ResourceDescriptor data_ov045_02112fdc = {
    data_ov045_02113220,
    data_ov045_02113228,
    data_ov045_021124f0
};

extern "C" {
int func_ov002_020b6244(daObjKm2_Gura_c *self, ResourceDescriptor *descriptor);
int func_ov002_020b60fc(daObjKm2_Gura_c *self, ResourceDescriptor *descriptor);
}

struct GuraSpawnInfo {
    daObjKm2_Gura_c *(*classInit)();
    s16 executePriority; /* +4 */
    s16 renderPriority;  /* +6 */
    u32 actorFlags;
    Fix12i clipOffsetY;
    Fix12i clipRadius;
    Fix12i clipDistance;
    Fix12i farDistance;
};
typedef char GuraSpawnInfo_size_must_be_0x1c[
    sizeof(GuraSpawnInfo) == 0x1c ? 1 : -1];

// @symbol daObjKm2_Gura_c_classInit
extern "C" daObjKm2_Gura_c *daObjKm2_Gura_c_classInit()
{
    return new daObjKm2_Gura_c();
}

// @symbol g_profile_KM2_GURA
extern "C" GuraSpawnInfo g_profile_KM2_GURA = {
    daObjKm2_Gura_c_classInit,
    0x008d,
    0x00d4,
    2,
    0x00120000,
    0x00200000,
    0x02000000,
    0
};

// @symbol _ZN15daObjKm2_Gura_c13InitResourcesEv
s32 daObjKm2_Gura_c::InitResources()
{
    return func_ov002_020b6244(this, &data_ov045_02112fdc);
}

// @symbol _ZN15daObjKm2_Gura_c16CleanupResourcesEv
s32 daObjKm2_Gura_c::CleanupResources()
{
    return func_ov002_020b60fc(this, &data_ov045_02112fdc);
}
