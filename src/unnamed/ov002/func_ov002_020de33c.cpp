//cpp
#include "Player.h"

extern "C" {
extern int _ZN6Player7IsStateERNS_5StateE(void *c, void *s);
extern int _ZN6Player11ChangeStateERNS_5StateE(void *c, void *s);
extern char data_ov002_021102bc[];
extern char data_ov002_02110484[];


}
int Player::func_ov002_020de33c(int a) {char* c = (char*)this;
  if(*(unsigned char*)(c+0x709)==0
     && !_ZN6Player7IsStateERNS_5StateE(c, data_ov002_021102bc)
     && !_ZN6Player7IsStateERNS_5StateE(c, data_ov002_02110484)){
    goto ok;
  }
  return 0;
ok:
  *(int*)(c+0x364) = a;
  _ZN6Player11ChangeStateERNS_5StateE(c, data_ov002_021102bc);
  return 1;

}
