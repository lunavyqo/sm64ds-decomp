#include "types.h"

/* The pad-record unit's allocation tag and the menu memory test's sentinel
 * word. The pad unit (func_ov007_020c2090..func_ov007_020c232c) is not yet
 * one folded translation unit, and the sentinel's only writer
 * func_ov007_020c3bc8 sits in a text-only shard, so these eight bytes keep
 * the data-file home they had before the fold. */
char data_ov007_0210324c[] = "pad";
int data_ov007_02103250[] = {0x98765432};
