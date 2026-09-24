//cpp
// @symbol _ZN8daFPkn_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and four destructor calls, every one a consequence of
 * `struct daFPkn_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "daFPkn_c.h"

daFPkn_c::~daFPkn_c()
{
}
