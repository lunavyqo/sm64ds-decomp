//cpp
/**
 * Wet-Dry World's spinning disc.
 *
 * No fields. InitResources / CleanupResources hand this overlay's
 * model and collision files to daObjKaitendai_c's shared ov002
 * helpers, plus a starting angle at data_ov029_02113fc4.
 *
 * daObjWc_Obj07_c_classInit is reconstructed (RTTI daObjWc_Obj07_c,
 * WC_UKISIMA registry). Retail does not store that spelling.
 *
 * deslop
 * Leftover: func_ov002_020b676c / func_ov002_020b66a8 are still the
 *   shared kaitendai setup/teardown; names are placeholders.
 * Leftover: data_ov029_02113fd4 is an opaque 3-word file table;
 *   data_ov029_02113fc4 is the start-angle halfword. Nothing in this
 *   TU dereferences either.
 * Leftover: in-class operator new takes unsigned long and forwards to
 *   _ZN7fBase_cnwEj (unsigned int). Keep `return new daObjWc_Obj07_c`.
 * Leftover: inline destructor (out-of-line emits D0 before D1).
 * Leftover: g_profile_WC_UKISIMA lives outside this TU (S14).
 * Leftover: `return new` emits base D2 helpers (kaitendai, dBgActor);
 *   deadstrip like daObjKi_Ita_c / daObjKm1_Kuruma_c.
 */

#include "daObjWc_Obj07_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov002_020b676c(daObjWc_Obj07_c *self, ResourceDescriptor *descriptor,
                        s16 angle);
int func_ov002_020b66a8(daObjWc_Obj07_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov029_02113fd4;
extern s16 data_ov029_02113fc4;
}

// @symbol daObjWc_Obj07_c_classInit
extern "C" daObjWc_Obj07_c *daObjWc_Obj07_c_classInit()
{
    return new daObjWc_Obj07_c();
}

// @symbol _ZN15daObjWc_Obj07_c13InitResourcesEv
s32 daObjWc_Obj07_c::InitResources()
{
    return func_ov002_020b676c(this, &data_ov029_02113fd4, data_ov029_02113fc4);
}

// @symbol _ZN15daObjWc_Obj07_c16CleanupResourcesEv
s32 daObjWc_Obj07_c::CleanupResources()
{
    return func_ov002_020b66a8(this, &data_ov029_02113fd4);
}
