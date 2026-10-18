//cpp
/* Which Wiggler (MG_HANACHAN): the scene factory and the constructor for
 * the five 8-byte entries inside each of its fifteen 0x98-byte elements.
 * The scene's own methods are in src/minigames/d_s_mg_hanachan_methods.cpp.
 *
 * leftovers:
 *   - dScMgHanachan_c has no constructor declared, so the factory builds
 *     the object by hand: operator new, base constructor, vtable word
 *     (data_ov006_0213cab8 is _ZTV15dScMgHanachan_c), array construction.
 *   - The plain Wiggler and Point records describe the observed layout.
 *     Their lifecycle callbacks retain their scalar ABI and manual array
 *     construction; the five points begin at +0x18.
 */

#include "dScMgHanachan_c.h"

extern "C" {
extern void *_ZN11dScMgBase_cC2Ev(void *p);
extern void __cxa_vec_ctor(void *obj, int a, int b, void *cb1, void *cb2);
extern int data_ov006_0213cab8[];
void *dScMgHanachan_c_classInit(void);
/* local extern: only sibling minigame headers (dScMgAmida_c.h and four
   others) declare it; this TU includes none of them */
extern void NullDestructor_0203d47c(void);
extern void func_0203d738(void);
}

// @symbol Wiggler_Construct
extern "C" void *Wiggler_Construct(char *elem) {
    __cxa_vec_ctor(((dScMgHanachan_c::Wiggler *)elem)->seg, 5, 8, (void *)func_0203d738,
                   (void *)NullDestructor_0203d47c);
    return elem;
}

// @symbol dScMgHanachan_c_classInit
extern "C" void *dScMgHanachan_c_classInit(void) {
    char *scene = (char *)fBase_c::operator new(0x4f68);
    if (scene != 0) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(int *)scene = (int)data_ov006_0213cab8;
        __cxa_vec_ctor(((dScMgHanachan_c *)scene)->mWiggler, 0xf, 0x98, (void *)Wiggler_Construct,
                       (void *)Wiggler_Destroy);
    }
    return scene;
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
extern "C" int data_ov006_0213cab8[36] = { 0x020edb04, 0x020b0930, 0x020b08f0, 0x020ecec8, 0x0202e5f0, 0x020b0840, 0x020ed18c, 0x020b0620, 0x0202e3c8, 0x020ecee4, 0x020b04f4, 0x0202e398, 0x020b04e8, 0x0204357c, 0x0204349c, 0x02043494, 0x020ea280, 0x020ea2c8, 0x020eda48, 0x020b2994, 0x020b2990, 0x020b298c, 0x020ae198, 0x020ae1a0, 0x020ae140, 0x020ae128, 0x020b04e0, 0x020af27c, 0x020af04c, 0x020af094, 0x020aeed8, 0x020b2880, 0x020b27f4, 0x020b265c, 0x020ae3b4, 0x020ad660 };
#pragma section forcedata end
