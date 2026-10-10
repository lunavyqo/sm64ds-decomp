//cpp
#include "Player.h"
extern "C" {
extern void _ZN6Player11ChangeStateERNS_5StateE(void*, void*);
extern char data_ov002_021101b4[];
}
void Player::func_ov002_020dd8f4() {
    void* player = this;
    _ZN6Player11ChangeStateERNS_5StateE(player, data_ov002_021101b4);
}
