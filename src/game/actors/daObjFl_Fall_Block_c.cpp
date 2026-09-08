//cpp
/**
 * Lethal Lava Land falling block.
 *
 * No extra state. InitResources / CleanupResources hand this stage's
 * model and collision files to the shared falling-block setup.
 *
 * deslop
 * Leftover: func_ov098_0213a794 / func_ov098_0213a2cc and
 * data_ov022_0211427c (shared falling-block setup lives in ov098;
 * this leaf only passes the file table).
 */

#include "daObjFl_Fall_Block_c.h"

struct ResourceDescriptor {
    void *entries[3];
};

extern "C" {
int func_ov098_0213a2cc(daObjFl_Fall_Block_c *self, ResourceDescriptor *descriptor);
int func_ov098_0213a794(daObjFl_Fall_Block_c *self, ResourceDescriptor *descriptor);
extern ResourceDescriptor data_ov022_0211427c;
}

// @symbol _ZN20daObjFl_Fall_Block_c13InitResourcesEv
int daObjFl_Fall_Block_c::InitResources()
{
    return func_ov098_0213a794(this, &data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_c16CleanupResourcesEv
int daObjFl_Fall_Block_c::CleanupResources()
{
    return func_ov098_0213a2cc(this, &data_ov022_0211427c);
}

// @symbol _ZN20daObjFl_Fall_Block_cD1Ev
// @symbol _ZN20daObjFl_Fall_Block_cD0Ev
