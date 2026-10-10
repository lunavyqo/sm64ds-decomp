//cpp
#include "Player.h"
extern "C" {
extern int data_ov002_021104cc[];
extern void _ZN6Player11ChangeStateERNS_5StateE(void*,void*);
}
int Player::func_ov002_020e2b6c(){
  char* c = (char*)this;
  if(*(int*)(c+0x68c) < 0xb000) return 0;
  _ZN6Player11ChangeStateERNS_5StateE(c,data_ov002_021104cc);
  return 1;
}
