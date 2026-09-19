//cpp
// @symbol _ZN9daPopoi_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * One vtable store and a destructor call per member, every one a consequence of
 * `struct daPopoi_c : dEnemyBase_c` and the members that declaration types, destroyed in
 * reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 *
 * This body is the evidence for the header: each member's size closes exactly
 * on the next one's offset.
 */
#include "daPopoi_c.h"

daPopoi_c::~daPopoi_c()
{
}
