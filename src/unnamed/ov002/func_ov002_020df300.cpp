//cpp
#include "Player.h"

extern "C" {
extern void func_0200d438(int a, unsigned char b);
extern int data_0209f318;



}
int Player::func_ov002_020df300() {char* p = (char*)this;
    if (*(unsigned char*)(p + 0x6e3)) return 0;
    *(unsigned char*)(p + 0x6e3) = 1;
    func_0200d438(data_0209f318, *(unsigned char*)(p + 0x6d8));
    return 1;

}
