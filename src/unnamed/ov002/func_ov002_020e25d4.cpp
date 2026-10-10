//cpp
// @symbol _ZN6Player19func_ov002_020e25d4Ev
/* recovered: shared common types */
#include "common.h"
#include "Player.h"


extern "C" void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const Vector3 *v);

void Player::func_ov002_020e25d4()
{
    char *c = (char *)this;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(unsigned char *)(c + 0x6d9), 0x10, (const Vector3 *)(c + 0x74));
}