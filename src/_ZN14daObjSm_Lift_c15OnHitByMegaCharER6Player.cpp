//cpp
// @symbol _ZN14daObjSm_Lift_c15OnHitByMegaCharER6Player
#include "daObjSm_Lift_c.h"
#include "Player.h"

/* daObjSm_Lift_c::OnHitByMegaChar -- vtable slot 27, ov018 0x0211123c.
 *
 * Real member function: Player::IncMegaKillCount and dBgActor_c::
 * KillByMegaChar are both real methods (include/Player.h,
 * include/dBgActor_c.h). unk_094/unk_08e are dActor_c's own mPrevAngleY/
 * mAngleY, real named fields (include/dActor_c.h); mAngleY is bumped by
 * 0x4000 (a quarter turn) off the previous frame's angle. */
void daObjSm_Lift_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    KillByMegaChar(player);
    mAngleY = mPrevAngleY + 0x4000;
}
