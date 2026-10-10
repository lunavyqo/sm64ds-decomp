//cpp
#include "Player.h"
struct State {};
struct dCamera_c {};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN6Player7IsStateERNS_5StateE(void *, State& s);
extern "C" void _ZN6Player11ChangeStateERNS_5StateE(void *, State& s);

/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern "C" int _ZN6Player19func_ov002_020e0478Ev(void* c);
extern "C" void func_0200d474(dCamera_c* thiz, unsigned char playerID);
extern State data_ov002_0210ffec;
extern State data_ov002_021102d4;
extern dCamera_c* data_0209f318;
int Player::func_ov002_020df34c() {
    unsigned char* p = (unsigned char*)this;
    if (p[0x709] != 0 || _ZN6Player19func_ov002_020e0478Ev(p) != 0 || _ZN6Player7IsStateERNS_5StateE(p, data_ov002_0210ffec) != 0) {
        return 0;
    }
    p[0x6e3] = 0;
    _ZN6Player11ChangeStateERNS_5StateE(p, data_ov002_021102d4);
    func_0200d474(data_0209f318, p[0x6d8]);
    return 1;
}
