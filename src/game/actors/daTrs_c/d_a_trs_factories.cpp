//cpp
/* The TERESA (Boo) and BOSS_TERESA (Big Boo) registry factories -- ov063/daTrs_c.
 *
 * Both profiles construct the ROM-proven daTrs_c class (size 0x5e0, vtable
 * _ZTV7daTrs_c): the init param's actor id (0xd1 ordinary, 0xd2 boss) selects
 * the shape InitResources builds, not the class. A class-anchored classInit
 * rename would collide, so each factory keeps its profile-suffixed spelling.
 *
 * The two bodies sit adjacent in the ROM's own .text order (BOSS_TERESA at
 * 0x0211c520, TERESA at 0x0211c590) with no gap and no other class between
 * them, bounded below by daTBasket_c's factory and above by daTrsTrap_c's
 * destructors. They are written here highest-address-first: mwccarm lays
 * .text down in reverse source order.
 *
 * C LINKAGE IS LOAD-BEARING -- the ROM symbols are the bare names.
 * `return new daTrs_c()` MATCHES: the leaf operator new forwards
 * `_ZN7fBase_cnwEj`, and the synthesized ctor stores the vtable between the
 * dCapEnemy_c base construction and the six member constructions, which is
 * the ROM's own order. Leaf operator new forwards until #2570.
 *
 * Reconstructed source-style names: SM64DS proves daTrs_c through RTTI,
 * allocation size, vtable identity, and the TERESA / BOSS_TERESA registry
 * profiles; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical aliases: Boo_Spawn, BigBoo_Spawn.
 */

#include "daTrs_c.h"

extern "C" {

// @symbol daTrs_c_classInit_TERESA
daTrs_c *daTrs_c_classInit_TERESA(void)
{
    return new daTrs_c();
}

// @symbol daTrs_c_classInit_BOSS_TERESA
daTrs_c *daTrs_c_classInit_BOSS_TERESA(void)
{
    return new daTrs_c();
}

}
