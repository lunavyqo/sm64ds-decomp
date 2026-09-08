#ifndef DAOBJFL_FALL_BLOCK_C_H
#define DAOBJFL_FALL_BLOCK_C_H

#include "types.h"
#include "daObjFallBlock_c.h"

/**
 * Lethal Lava Land falling block.
 *
 * A daObjFallBlock_c leaf: no fields of its own. InitResources and
 * CleanupResources load and free this stage's files.
 */

#ifdef __cplusplus

struct daObjFl_Fall_Block_c : daObjFallBlock_c {
    virtual ~daObjFl_Fall_Block_c() {}
    int CleanupResources();
    int InitResources();
};

typedef char daObjFl_Fall_Block_c_size_must_be_0x34c[sizeof(daObjFl_Fall_Block_c) == 0x34c ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJFL_FALL_BLOCK_C_H */
