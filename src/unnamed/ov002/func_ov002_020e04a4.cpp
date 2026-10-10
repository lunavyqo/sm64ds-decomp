//cpp
#include "Player.h"
#include "PlayerInput.h"

extern "C" {
/* _ZN6Player19func_ov002_020e04a4Ev at 0x020e04a4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov002).
 */

extern void _ZN6Player19func_ov002_020bf88cEv(char* c);
extern short data_0209f4a0[];


}
void Player::func_ov002_020e04a4() {char* c = (char*)this;
  if (*(short*)((char*)data_0209f4a0 + gActivePlayerSlot*0x18) < 0x600) return;
  _ZN6Player19func_ov002_020bf88cEv(c);

}
