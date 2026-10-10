//cpp
#include "Player.h"
#include "decl_common.h"
#include "common.h"

extern "C" {
// @symbol _ZN6Player19func_ov002_020dbc94Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
extern unsigned char data_0209f2d8;





}
void Player::func_ov002_020dbc94() {char* c = (char*)this;
    struct Vector3 v;
    int b = (int)(data_0209f2d8 == 1);
    if (b) {
        int r2 = 0x82000;
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        if (*(int*)(c + 8) == 2) r2 = 0xc8000;
        _ZN6Player19func_ov002_020dc174EPviijj(c, &v, r2, 0x5a000, 0x20, 0);
    } else if (*(unsigned char*)(c + 0x703) == 0) {
        v.x = 0;
        v.y = -0x32000;
        v.z = 0;
        _ZN6Player19func_ov002_020dc174EPviijj(c, &v, 0x32000, 0x5a000, 0x20, 0);
    } else {
        v.x = 0;
        v.y = -0x40000;
        v.z = 0;
        _ZN6Player19func_ov002_020dc174EPviijj(c, &v, 0xc0000, 0x80000, 0x20, 0);
    }

}
