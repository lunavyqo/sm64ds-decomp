//cpp
/**
 * Rainbow Ride's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers. Start angle is data_ov036_02113b18, or
 * data_ov036_02113b1c when param1's low byte is 1.
 *
 * daObjRc_Kaitendai_c_classInit is reconstructed (RTTI
 * daObjRc_Kaitendai_c, RC_KAITEN registry). Retail does not store
 * that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still the
 *   shared kaitendai setup/teardown; names are placeholders.
 * Leftover: data_ov036_02113b2c is an opaque 3-word file table;
 *   the two angle halfwords are still data_ov036_*. Nothing in this
 *   TU dereferences the table.
 * Leftover: g_profile_RC_KAITEN lives outside this TU (S14).
 * Leftover: `return new` emits base D2 helpers (kaitendai, dBgActor);
 *   deadstrip like daObjBk_Ukisima_c / daObjWc_Obj07_c.
 */

#include "daObjRc_Kaitendai_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b676c(daObjRc_Kaitendai_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
int func_ov002_020b66a8(daObjRc_Kaitendai_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov036_02113b2c;
extern s16 data_ov036_02113b18;
extern s16 data_ov036_02113b1c;
}

// @symbol daObjRc_Kaitendai_c_classInit
extern "C" daObjRc_Kaitendai_c *daObjRc_Kaitendai_c_classInit()
{
    return new daObjRc_Kaitendai_c();
}

// @symbol _ZN19daObjRc_Kaitendai_c13InitResourcesEv
s32 daObjRc_Kaitendai_c::InitResources()
{
    s16 angle = data_ov036_02113b18;
    if ((param1 & 0xff) == 1)
        angle = data_ov036_02113b1c;
    return func_ov002_020b676c(this, &data_ov036_02113b2c, angle);
}

// @symbol _ZN19daObjRc_Kaitendai_c16CleanupResourcesEv
s32 daObjRc_Kaitendai_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov036_02113b2c);
}
