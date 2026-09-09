#ifndef DAOBJBK_FALL_BLOCK_C_H
#define DAOBJBK_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Bob-omb Battlefield's falling block. No fields of its own: the
 * factory allocates 0x34c, which daObjFallBlock_c fills. Overrides
 * the two slots the base leaves null (InitResources,
 * CleanupResources) and hands this overlay's model/collision
 * descriptor to the shared ov098 helpers.
 *
 * `daObjBk_Fall_Block_c` is the RTTI name.
 */
struct daObjBk_Fall_Block_c : daObjFallBlock_c {
    /* Inline and first: out-of-line mwccarm emits D0 before D1; retail
       has D1 at 0x02112cf4 below D0 at 0x02112d44. */
    virtual ~daObjBk_Fall_Block_c() {}
    virtual s32 CleanupResources(); /* slot 3 */
    virtual s32 InitResources();    /* slot 0 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj(size);
    }
};

typedef char daObjBk_Fall_Block_c_size_must_be_0x34c[
    sizeof(daObjBk_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* DAOBJBK_FALL_BLOCK_C_H */
