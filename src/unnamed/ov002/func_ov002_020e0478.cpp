//cpp
#include "Player.h"
extern "C" {
extern int data_ov002_02110214[];
int _ZN6Player7IsStateERNS_5StateE(void*, void*);
}
int Player::func_ov002_020e0478(){
  void* c = this;
  return _ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110214) != 0;
}
