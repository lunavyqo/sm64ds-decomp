//cpp
#include "Player.h"
extern "C" {
struct State;
extern State data_ov002_021105a4;
extern int _ZN6Player7IsStateERNS_5StateE(void* c, State* st);
}
int Player::func_ov002_020dd8b8(){
  char* c = (char*)this;
  if(!_ZN6Player7IsStateERNS_5StateE(c, &data_ov002_021105a4)) return 0;
  return *(unsigned char*)(c+0x6e3)==0 ? 1 : 0;
}
