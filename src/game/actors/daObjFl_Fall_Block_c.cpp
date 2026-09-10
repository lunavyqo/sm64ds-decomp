//cpp
/**
 * Lethal Lava Land falling block.
 *
 * No extra state. InitResources / CleanupResources hand this stage's
 * model and collision files to the shared falling-block setup.
 *
 * deslop
 * Leftover: func_ov098_0213a794 / func_ov098_0213a2cc are still the
 *   linker names of daObjFallBlock_c Init/Cleanup. Naming belongs
 *   in ov098. That helper loads slot 0 with Model::LoadFile, slot 1
 *   with dBgW_Kc::LoadFile, slot 2 as CLPS into SetFile.
 * Leftover: factory is still src/d_a_obj_fl_fall_block.c, outside
 *   this TU.
 */

#include "daObjFl_Fall_Block_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

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
