#ifndef DAOBJFL_GURA_C_H
#define DAOBJFL_GURA_C_H

#include "types.h"
#include "daObjGuragura_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Lethal Lava Land's tilting slab. No fields of its own: the
 * factory allocates 0x350, which daObjGuragura_c fills. Overrides
 * the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov002 helpers.
 *
 * `daObjFl_Gura_c` is the RTTI name. MetalNetLift in this overlay
 * is a different class.
 */
struct daObjFl_Gura_c : daObjGuragura_c {
    /* Declared last and inline so class instantiation emits the
       retail D1/D0 pair in cartridge order without a separate leaf
       D2 body. */
    int CleanupResources();            /* slot  3 */
    int InitResources();               /* slot  0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }

    virtual ~daObjFl_Gura_c() {}
};

typedef char daObjFl_Gura_c_size_must_be_0x350[
    sizeof(daObjFl_Gura_c) == 0x350 ? 1 : -1];

#endif /* DAOBJFL_GURA_C_H */
