//cpp
/* arm9/Sound2 -- the second Sound::Func_* run at .text 0x02048e94..0x02048ef4,
 * folded from five legacy shards. The run is three wrappers around the
 * shared func_02048fd4 worker plus the two address-named helpers
 * that call it: func_02048e94 arms the player-count global and forwards to
 * Sound::Player::SetPlayableSeqCount, func_02048ed4 forwards 6. The Func_
 * wrappers keep their literal mangled names under extern "C", the same
 * spelling the home TU src/engine/sound/Sound.cpp already declares them
 * with. That home TU exists but is not contiguous with this run
 * (other files sit in between), so this is its own original file. The span
 * is bounded on both sides by unnamed helpers: func_02048e74.c ends at
 * 0x02048e94 below and func_02048ef4.c begins at 0x02048ef4 above.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   Sound::Func_02048ee4, func_02048ed4, Sound::Func_02048ec4,
 *     Sound::Func_02048eb4, func_02048e94 -- each in its own extern "C"
 *   block at its address position.
 */

#include "types.h"

extern "C" {
extern void func_02048fd4(int i);
extern int _ZN5Sound6Player19SetPlayableSeqCountEii(int a, int b);
extern int data_02099fb0;
}

// @symbol _ZN5Sound13Func_02048ee4Ev
extern "C" void _ZN5Sound13Func_02048ee4Ev(void)
{
    func_02048fd4(5);
}

extern "C" {
// @symbol func_02048ed4
void func_02048ed4(void)
{
    func_02048fd4(6);
}
}

// @symbol _ZN5Sound13Func_02048ec4Ev
extern "C" void _ZN5Sound13Func_02048ec4Ev(void)
{
    func_02048fd4(7);
}

// @symbol _ZN5Sound13Func_02048eb4Ev
extern "C" void _ZN5Sound13Func_02048eb4Ev(void)
{
    func_02048fd4(0);
}

extern "C" {
// @symbol func_02048e94
int func_02048e94(void) {
    data_02099fb0 = 4;
    return _ZN5Sound6Player19SetPlayableSeqCountEii(9, 4);
}
}
