//cpp
/**
 * Bob-omb Battlefield's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov015_02114794.
 *
 * daObjBk_Ukisima_c_classInit is reconstructed (RTTI daObjBk_Ukisima_c,
 * BK_UKISIMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still the
 *   shared kaitendai setup/teardown; names are placeholders.
 * Leftover: data_ov015_021147a4 is an opaque 3-word file table;
 *   data_ov015_02114794 is the start-angle halfword. Nothing in this
 *   TU dereferences either.
 * Leftover: in-class operator new must be unsigned long (unsigned
 *   int is `illegal 'operator' declaration`). fBase's is unsigned
 *   int (`nwEj`). Keep `return new daObjBk_Ukisima_c`.
 * Leftover: inline destructor. Out-of-line still MATCHES D1/D0
 *   bytes but emits D0 before D1 and a homeless leaf D2.
 * Leftover: g_profile_BK_UKISIMA lives outside this TU (S14).
 * Leftover: `return new` emits base D2 helpers (kaitendai, dBgActor);
 *   deadstrip like daObjWc_Obj07_c / daObjKi_Ita_c.
 */

#include "daObjBk_Ukisima_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b676c(daObjBk_Ukisima_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
int func_ov002_020b66a8(daObjBk_Ukisima_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov015_021147a4;
extern s16 data_ov015_02114794;
}

// @symbol daObjBk_Ukisima_c_classInit
extern "C" daObjBk_Ukisima_c *daObjBk_Ukisima_c_classInit()
{
    return new daObjBk_Ukisima_c();
}

// @symbol _ZN17daObjBk_Ukisima_c13InitResourcesEv
s32 daObjBk_Ukisima_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov015_021147a4, data_ov015_02114794);
}

// @symbol _ZN17daObjBk_Ukisima_c16CleanupResourcesEv
s32 daObjBk_Ukisima_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov015_021147a4);
}
