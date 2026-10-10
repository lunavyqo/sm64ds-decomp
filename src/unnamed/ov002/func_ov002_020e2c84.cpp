//cpp
#include "Player.h"
#include "types.h"
#include "decl_common.h"
#include "common.h"

extern "C" {
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern void _ZN6Player19func_ov002_020e301cEv(char*);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e3078EPv(char*, void*);
/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020c0cbcEv(char*);
// @symbol _ZN6Player19func_ov002_020e2c84Ev
/* recovered: shared common types, declarations from a shared header */
/* recovered: shared common types */
struct dCamera_c;

/* local extern: caller passes the player pointer explicitly; Player.h is C++-only */
extern int _ZN6Player19func_ov002_020e3078EPv(char *self, void *s);
extern int func_ov002_020e2ea0(char *self);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *self, void *state);
extern int func_ov002_020d91e0(char *thiz, int damage, int doPre);
extern void func_0200d8c8(struct dCamera_c *cam, const struct Vector3 *v, int strength);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, const struct Vector3 *v);
extern int _ZN6Player7IsStateERNS_5StateE(char *self, void *state);
extern int func_ov002_020c5dec(char *c, int r1);
extern void func_ov002_020db8bc(unsigned char *p, unsigned char val);

extern char data_ov002_02110454;
extern char data_ov002_02110094;
extern char data_ov002_0211010c;
extern struct dCamera_c *data_0209f318;



}
int Player::func_ov002_020e2c84() {char * self = (char *)this;
    int diff;

    diff = *(int *)(self + 0x684) - *(int *)(self + 0x60);
    _ZN6Player19func_ov002_020e301cEv(self);

    if (_ZN6Player19func_ov002_020e3078EPv(self, &data_ov002_02110454) != 0 ||
        _ZN6Player19func_ov002_020e3078EPv(self, &data_ov002_02110484) != 0 ||
        *(u8 *)(self + 0x703) != 0)
        return 0;

    if (func_ov002_020e2ea0(self) != 0)
        return 2;

    if (diff > 0xbb8000) {
        *(u8 *)(self + 0x6e3) = 2;
        *(int *)(self + 0x674) = 0;
        _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_02110094);

        if (func_ov002_020d91e0(self, 0x400, 1) != 0) {
            {
                u8 *p = (u8 *)(self + 0x6e3);
                *p &= 1;
            }
            _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211010c);
            return 1;
        }

        func_0200d8c8(data_0209f318, (struct Vector3 *)(self + 0x5c), 0x7d0000);
        *(u8 *)(self + 0x70d) = 0;
        _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 7, (struct Vector3 *)(self + 0x74));
    } else if (diff > 0x47e000) {
        if (_ZN6Player19func_ov002_020c0cbcEv(self) == 0) {
            if (func_ov002_020d91e0(self, 0x200, 1) != 0) {
                if (_ZN6Player7IsStateERNS_5StateE(self, &data_ov002_02110094) != 0) {
                    {
                        u8 *p = (u8 *)(self + 0x6e3);
                        *p &= 1;
                    }
                    _ZN6Player11ChangeStateERNS_5StateE(self, &data_ov002_0211010c);
                } else {
                    func_ov002_020c5dec(self, 2);
                }
                return 1;
            }

            func_ov002_020db8bc((unsigned char *)self, 1);
            *(int *)(self + 0xa8) = 0;
            func_0200d8c8(data_0209f318, (struct Vector3 *)(self + 0x5c), 0x7d0000);
            *(u8 *)(self + 0x70d) = 0;
            _ZN5Sound13PlayCharVoiceEjjRK7Vector3(*(u8 *)(self + 0x6d9), 7, (struct Vector3 *)(self + 0x74));
        }
    }

    *(int *)(self + 0x684) = *(int *)(self + 0x60);
    return 0;

}
