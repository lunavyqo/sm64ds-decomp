//cpp
// @symbol _ZN6Player16St_BackFlip_InitEv
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int Fix12i;
extern "C" {
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e2be4Ev(void*);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e2ba8Ev(void*);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e2b6cEv(void*);
extern int _ZN6Player7SetAnimEji5Fix12IiEj(void*, unsigned int, int, Fix12i, unsigned int);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e2ad0Ev(void*);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e25f0Ei(void*, int);
}

int Player::St_BackFlip_Init()
{
  *(char*)((char*)&mJumpedFromQuicksand)=0;
  if (_ZN6Player19func_ov002_020e2be4Ev(((void*)this))) return 1;
  if (_ZN6Player19func_ov002_020e2ba8Ev(((void*)this))) return 1;
  if (_ZN6Player19func_ov002_020e2b6cEv(((void*)this))) return 1;
  *(char*)((char*)&mIsInAirState)=1;
  *(char*)((char*)&mIsFallScreaming)=0;
  *(char*)((char*)&mJumpComboStage)=0;
  *(char*)((char*)&mIsAirborne)=1;
  *(char*)((char*)&mLandSoundPlayed)=0;
  _ZN6Player7SetAnimEji5Fix12IiEj(((void*)this), 0x2a, 0x40000000, 0x1000, 0);
  *(int*)((char*)&mVertSpeed)=0x3e000;
  if (*(int*)((char*)&param1)==1) *(int*)((char*)&mVertSpeed)=0x4c000;
  _ZN6Player19func_ov002_020e2ad0Ev(((void*)this));
  *(int*)((char*)&mHorzSpeed)=0x10000;
  *(short*)((char*)&mPrevAngleY) = *(short*)((char*)&mAngleY) + 0x8000;
  _ZN6Player19func_ov002_020e25f0Ei(((void*)this), 0);
  return 1;
}
