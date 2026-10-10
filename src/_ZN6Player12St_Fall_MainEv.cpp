//cpp
// @symbol _ZN6Player12St_Fall_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern "C" {
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e28d4Eii(void*, int, int);
extern int _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern int Player_AdvanceAnims(void*);
extern Fix12i Player_ScaleByCharFactor(void* c, Fix12i a);
}
extern char data_ov002_02110424[];

int Player::St_Fall_Main()
{
  _ZN6Player19func_ov002_020e28d4Eii(((void*)this), 0x1800, 0x800);
  if (*(unsigned char*)((char*)&mIsAirborne) == 0) {
    _ZN6Player11ChangeStateERNS_5StateE(((void*)this), data_ov002_02110424);
  } else if (_ZN6Player19func_ov002_020e2664Ev(((void*)this))) {
    return 1;
  }
  Player_AdvanceAnims(((void*)this));
  if (*(int*)((char*)&mHorzSpeed) >= Player_ScaleByCharFactor(((void*)this), 0x28000))
    *(int*)((char*)&mHorzSpeed) = Player_ScaleByCharFactor(((void*)this), 0x28000);
  return 1;
}
