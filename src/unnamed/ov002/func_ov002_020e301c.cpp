//cpp
#include "Player.h"

extern "C" {
extern void func_0200d8c8(void* cam, void* v, int strength);
extern void* data_0209f318;


}
void Player::func_ov002_020e301c() {char* c = (char*)this;
  if(*(int*)(c+8) != 2) return;
  if(*(int*)(c+0x684) - *(int*)(c+0x60) < 0x190000) return;
  func_0200d8c8(data_0209f318, c+0x5c, 0x320000);

}
