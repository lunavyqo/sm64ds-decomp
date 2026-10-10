//cpp
#include "Player.h"

extern "C" {
extern int _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void*,void*,void*,int,int,int,int);


}
void Player::func_ov002_020dc174(void* r1, int r2, int r3, unsigned int a5, unsigned int a6) {char* c = (char*)this;
  int f = *(unsigned char*)(c+0x703) ? 0x10 : 0;
  _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(c+0x314, c, r1, r2, r3, a5|2|f, a6);

}
