//cpp
#include "Player.h"
#include "types.h"
#include "common.h"

extern "C" {
// @symbol _ZN6Player19func_ov002_020de428Ev
/* recovered: shared common types */
struct LocAct {
    char pad74[0x74];
    struct Vector3 vec; /* 0x74 */
    char pad[0x70c - 0x80];
    unsigned char flag; /* 0x70c */
};

extern void _ZN5Sound9PlayBank0EjRK7Vector3(u32 soundID, const struct Vector3 *pos);



}
void Player::func_ov002_020de428() {struct LocAct * thiz = (struct LocAct *)this;
    if (thiz->flag)
        return;
    _ZN5Sound9PlayBank0EjRK7Vector3(0xbd, &thiz->vec);
    thiz->flag = 0x1e;

}
