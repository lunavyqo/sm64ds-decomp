//cpp
#include "Player.h"

extern "C" {
extern unsigned char data_0209f2d8;
struct V3 { int a, b, c; };
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern void _ZN6Player19func_ov002_020dc174EPviijj(char* c, struct V3* r1, int r2, int r3, unsigned int a5, unsigned int a6);


}
void Player::func_ov002_020dbaec() {char* c = (char*)this;
  struct V3 v;
  int r2, r3;
  if((int)(data_0209f2d8 == 1) != 0){
    v.c = 0xa0000;
    v.a = 0;
    r2 = 0x78000;
    v.b = 0;
    v.c = 0x78000;
    r3 = 0x50000;
  }else if(*(unsigned char*)(c+0x703) == 0){
    r2 = 0x78000;
    v.c = 0x78000;
    r3 = 0x50000;
    v.a = 0;
    v.b = 0;
    r2 = r3;
    if(*(int*)(c+8) == 2){
      v.c = 0xa0000;
      r2 = 0x78000;
    }
  }else{
    v.a = 0;
    v.b = -0x40000;
    v.c = 0x80000;
    r3 = 0x100000;
    r2 = 0xc0000;
  }
  _ZN6Player19func_ov002_020dc174EPviijj(c, &v, r2, r3, 0x200, 0);

}
