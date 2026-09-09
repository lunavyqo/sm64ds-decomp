//cpp
/**
 * Bob-omb Battlefield's falling block.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjFallBlock_c's shared ov098
 * helpers.
 *
 * daObjBk_Fall_Block_c_classInit is reconstructed (RTTI
 * daObjBk_Fall_Block_c, BK_DOWN_B registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov098_0213a794 / func_ov098_0213a2cc are still the
 *   shared falling-block setup/teardown; names are placeholders.
 * Leftover: data_ov015_02114880 is an opaque 3-word file table.
 *   Nothing in this TU dereferences it.
 * Leftover: g_profile_BK_DOWN_B lives outside this TU (S14).
 */

#include "daObjBk_Fall_Block_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov098_0213a794(daObjBk_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a2cc(daObjBk_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov015_02114880;
}

// @symbol daObjBk_Fall_Block_c_classInit
extern "C" daObjBk_Fall_Block_c *daObjBk_Fall_Block_c_classInit()
{
    return new daObjBk_Fall_Block_c();
}

// @symbol _ZN20daObjBk_Fall_Block_c13InitResourcesEv
s32 daObjBk_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov015_02114880);
}

// @symbol _ZN20daObjBk_Fall_Block_c16CleanupResourcesEv
s32 daObjBk_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov015_02114880);
}
