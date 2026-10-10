//cpp
#include "Player.h"
#include "decl_common.h"
#include "common.h"

extern "C" {
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern void _ZN6Player19func_ov002_020dd8f4Ev(char*);
// @symbol _ZN6Player19func_ov002_020dd908Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern int Vec3_Dist(const struct Vector3* a, const struct Vector3* b);
extern unsigned char data_0209f2d8;
extern unsigned char data_0209f21c;
extern char* data_0209f394[];


}
void Player::func_ov002_020dd908() {char* sb = (char*)this;
  int i;
  struct Vector3 v;
  if((int)(data_0209f2d8 == 1) == 0) return;
  for(i = 0; i < data_0209f21c; i++){
    char* o = data_0209f394[i];
    int* p;
    if(o == 0) continue;
    if(*(int*)(o+0x37c) == 0) continue;
    p = (int*)(((int)(o+0x5c)));
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    v.y = *(int*)(sb+0x60);
    if(Vec3_Dist((struct Vector3*)(sb+0x5c), &v) >= 0x12c000) continue;
    _ZN6Player19func_ov002_020dd8f4Ev(o);
  }

}
