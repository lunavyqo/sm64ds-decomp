//cpp
/**
 * Battler block (BATTA_BLOCK). The cartridge RTTI is daBttBk_c: _ZTS at
 * ov080 0x0212815c is "9daBttBk_c", _ZTI at 0x02128168 is
 * [__si_class_type_info+8, that string, _ZTI8dActor_c], and the vtable
 * preamble word at 0x02128194 is that same _ZTI. The address point
 * _ZTV is 0x02128198 (slot 0, InitResources).
 *
 * .text 0x02124a20..0x021253b4, twenty functions. The factory
 * daBttBk_c_classInit is the next function (0x021253b4) and stays in its
 * own TU: every other unit in ov080 ends on a classInit, and this one is
 * not part of the daBttBk_c run tubuild cuts.
 *
 * Emission is ROM-ascending under `#pragma defer_codegen off`. The
 * destructor is defined once, out of line; mwccarm emits D1 then D0.
 * D2 has no ROM home.
 */

/* decl_common.h spells func_ov080_0212513c as one argument. The body
 * takes the state index too. Hide that declaration; this TU is the
 * only caller. */
#define func_ov080_0212513c func_ov080_0212513c_hidden_decl
#include "decl_common.h"
#undef func_ov080_0212513c
extern "C" void func_ov080_0212513c(char *c, int i);
#include "daBttBk_c.h"
#include "dBgCh_Gnd.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "Player.h"

struct Mtx43 { int a[12]; };
typedef char Mtx43_size_must_be_0x30[sizeof(Mtx43) == 0x30 ? 1 : -1];

struct C;
typedef void (C::*PMF)();
struct C { char pad[0x36c]; PMF *pp; };
typedef char C_size_must_be_0x370[sizeof(C) == 0x370 ? 1 : -1];

extern "C" {
void *func_02010304(void *a, void *b);
void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player16IncMegaKillCountEv(void *p);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN8dActor_c10PoofDustAtERK7Vector3(void *self, const Vector3 &vec);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiES(void *self, const Vector3 &v, unsigned int n, int vel, short unk);
void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, const Vector3 &pos);
void _ZN7fBase_c18MarkForDestructionEv(void *self);
void _ZN5dCc_c5ClearEv(void *);
void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *m, int x, int y, int z);
void Vec3_LslInPlace(Vector3 *v, int sh);
void Matrix4x3_FromRotationY(void *m, int angle);
void _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
    void *self, void *sm, void *mtx, int r, int t5, int t6, unsigned int u);
void dBgCh_Actr_UpdateDiscreteNoLava_veneer(dBgCh_Actr *w);
void _ZN10dBgCh_Actr13SetLimMovFlagEv(void *);
void *_ZN5Model8LoadFileER13SharedFilePtr(void *fp);
void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *self, void *file, int a, int b);
void _ZN11ShadowModel10InitCuboidEv(void *self);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *actor, int a, int b, unsigned int c, unsigned int d);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, int a, int b, void *v, int c);
extern void func_ov080_02124c3c(char *c);
}

extern Mtx43 data_020a0e68;
extern s16 data_02082214[];
extern char data_ov080_0212847c[];

/* InitResources and func_ov080_02124e60 leave r1 holding the 0 they just
 * stored. A two-arg call would write r1 again. */
typedef void (*Fn2513c1)(char *);

#pragma defer_codegen off

// @symbol _ZN9daBttBk_cD1Ev
// @symbol _ZN9daBttBk_cD0Ev
/* Empty on purpose. mwccarm destroys the members, then ~dActor_c, and
 * emits retail D1 followed by D0. */
daBttBk_c::~daBttBk_c()
{
}

// @symbol _ZN9daBttBk_c13OnYoshiTryEatEv
s32 daBttBk_c::OnYoshiTryEat()
{
    return 1;
}

// @symbol func_ov080_02124acc
extern "C" void func_ov080_02124acc(char *c)
{
    if (*(int *)(c + 0x170) == 0) return;
    void *p = func_02010304(c, c + 0x14c);
    if (p != 0) { *(void **)(c + 0x374) = p; func_ov080_0212513c(c, 1); return; }
    void *a = _ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x170));
    if (a == 0) return;
    int b1 = (int)(*(unsigned short *)((char *)a + 0xc) == 0xbf);
    if (b1 == 0) return;
    int b2 = (int)((*(unsigned int *)(c + 0xb0) & 0x20000) != 0);
    if (b2) { func_ov080_0212513c(c, 2); return; }
    if ((*(unsigned int *)(c + 0x16c) & 0x10) == 0) return;
    _ZN6Player16IncMegaKillCountEv(a);
    Vector3 v; Vector3 v2; Vector3 v3;
    int y0 = *(int *)(c + 0x60);
    int z = *(int *)(c + 0x64);
    int x = *(int *)(c + 0x5c);
    int y = y0 + 0x32000;
    v.x = x; v.y = y; v.z = z;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, v.x, v.y, v.z);
    ((int *)&v2)[0] = ((int *)&v)[0];
    ((int *)&v2)[1] = ((int *)&v)[1];
    ((int *)&v2)[2] = ((int *)&v)[2];
    _ZN8dActor_c10PoofDustAtERK7Vector3(c, v2);
    int t = *(int *)(c + 0x60) + 0x64000;
    v3.x = v.x;
    v.y = t;
    v3.y = t;
    v3.z = v.z;
    _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiES(c, v3, 5, 0xf000, 0);
    _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3 *)(c + 0x74));
    _ZN7fBase_c18MarkForDestructionEv(c);
}

// @symbol func_ov080_02124c3c
extern "C" void func_ov080_02124c3c(char *c)
{
    Vector3 t;
    Vector3 pos;
    int flags = *(int *)(c + 0xb0);
    int gb = (flags & 0x40000) != 0;
    if (gb != false) return;
    void *p = *(void **)(c + 0x374);
    if (p != 0) {
        int gb2 = (flags & 0x4000) != 0;
        if (gb2 != false) {
            if (*(int *)((char *)p + 0xc8) != 0) {

                int tx = 0xc000, ty = 0x2000, tz = 0;
                ((int *)&t)[0] = tx; ((int *)&t)[1] = ty; ((int *)&t)[2] = tz;
                p = *(void **)(c + 0x374);
                data_020a0e68 = *(Mtx43 *)(*(void **)((char *)p + 0xc8));
                Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

                *(int *)(c + 0x5c) = data_020a0e68.a[9];
                *(int *)(c + 0x60) = data_020a0e68.a[10];
                *(int *)(c + 0x64) = data_020a0e68.a[11];
                Vec3_LslInPlace((Vector3 *)(c + 0x5c), 3);
            }
        }
    }
    Matrix4x3_FromRotationXYZExt(c + 0xf0, *(s16 *)(c + 0x8c), *(s16 *)(c + 0x8e), *(s16 *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    pos.x = *(int *)(c + 0x5c);
    pos.y = *(int *)(c + 0x60);
    pos.z = *(int *)(c + 0x64);
    pos.y = pos.y + 0x14000;
    dBgCh_Gnd rg;
    rg.SetObjAndPos(pos, 0);
    int h = pos.y;
    if (rg.DetectClsn()) {
        h = rg.clsnY;
    }
    Matrix4x3_FromRotationY(c + 0x33c, *(s16 *)(c + 0x8e));
    *(int *)(c + 0x360) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x364) = h >> 3;
    *(int *)(c + 0x368) = *(int *)(c + 0x64) >> 3;
    {
        s16 a = *(s16 *)(c + 0x8c);
        int sv = data_02082214[((unsigned short)(short)(a << 1) >> 4) * 2];
        if (sv < 0) sv = -sv;
        int result = (int)(((s64)sv * 0x28000 + 0x800) >> 12);
        _ZN8dActor_c18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
            c, c + 0x124, c + 0x33c, 0x96000, 0x32000, result + 0x96000, 0xf);
    }
}

// @symbol func_ov080_02124e60
extern "C" int func_ov080_02124e60(char *c)
{
    int v = *(int *)(c + 0xb0);
    int b1 = (v & 0x20000) ? 1 : 0;
    if (b1 != 0) goto done;
    int b2 = (v & 0x40000) ? 1 : 0;
    if (b2 != 0) goto done;
    *(int *)(c + 0xd0) = 0;
    ((Fn2513c1)func_ov080_0212513c)(c);
done:
    return 1;
}

// @symbol func_ov080_02124eb0
extern "C" int func_ov080_02124eb0(char *c)
{
    *(int *)(c + 0x98) = 0;
    _ZN5dCc_c5ClearEv((char *)c + 0x14c);
    *(int *)(c + 0x370) = 2;
    return 1;
}

// @symbol func_ov080_02124edc
extern "C" int func_ov080_02124edc(char *c)
{
    *(short *)(c + 0x8c) = *(short *)(*(char **)(c + 0x374) + 0x8c);
    *(short *)(c + 0x8e) = *(short *)(*(char **)(c + 0x374) + 0x8e);
    *(short *)(c + 0x92) = *(short *)(c + 0x8c);
    *(short *)(c + 0x94) = *(short *)(c + 0x8e);
    {
        int f = *(int *)(c + 0xb0);
        int a = (int)((f & 0x100) != 0);
        if (a != 0) {
            int b = (int)((f & 0x2000) != 0);
            if (b == 0) goto clear;
        }
        {
            Vector3 vec;
            Vector3 vec2;
            int x = *(int *)(c + 0x5c);
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0xb4000;
            vec.x = x;
            vec.y = y;
            vec.z = z;
            ((int *)&vec2)[0] = ((int *)&vec)[0];
            ((int *)&vec2)[1] = ((int *)&vec)[1];
            ((int *)&vec2)[2] = ((int *)&vec)[2];
            _ZN8dActor_c10PoofDustAtERK7Vector3(c, vec2);
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);

            {
                int ybase = *(int *)(c + 0x60);
                int xx = vec.x;
                int y2 = ybase + 0x64000;
                int zz = vec.z;
                Vector3 v3;
                v3.x = xx;
                v3.z = zz;
                vec.y = y2;
                v3.y = y2;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiES(c, v3, 5, 0xf000, 0);
            }

            _ZN5Sound9PlayBank3EjRK7Vector3(0x41, *(Vector3 *)(c + 0x74));
            _ZN7fBase_c18MarkForDestructionEv(c);
        }
    }
clear:
    _ZN5dCc_c5ClearEv(c + 0x14c);
    return 1;
}

// @symbol func_ov080_02124fec
extern "C" int func_ov080_02124fec(int *c)
{
    int *r = (int *)(((int)c + 0xb0));
    int one = 1;
    *(int *)((char *)c + 0x370) = one;
    *r &= ~3;
    return one;
}

// @symbol func_ov080_0212500c
extern "C" int func_ov080_0212500c(char *c)
{
    dBgCh_Actr_UpdateDiscreteNoLava_veneer((dBgCh_Actr *)(c + 0x180));
    if (((dBgCh_Actr *)(c + 0x180))->JustHitGround()) {
        int v = *(int *)(c + 0xa8) * -0x3c;
        *(int *)(c + 0xa8) = v / 100;
    } else if (((dBgCh_Actr *)(c + 0x180))->IsOnGround()) {
        *(int *)(c + 0xa8) = 0xc000;
    }
    ((dActor_c *)c)->UpdatePos((dCc_c *)(c + 0x14c));
    func_ov080_02124acc(c);
    ((dCc_c *)(c + 0x14c))->Clear();
    ((dCc_c *)(c + 0x14c))->Update();
    return 1;
}

// @symbol func_ov080_0212509c
extern "C" int func_ov080_0212509c(char *c)
{
    *(int *)(c + 0xa8) = 49152;
    _ZN10dBgCh_Actr13SetLimMovFlagEv((char *)c + 0x180);
    *(int *)(c + 0x370) = 0;
    return 1;
}

// @symbol func_ov080_021250c8
extern "C" void func_ov080_021250c8(char *raw)
{
    C *c = (C *)raw;
    PMF *p = c->pp + 1;
    (c->**p)();
}

// @symbol func_ov080_02125104
extern "C" void func_ov080_02125104(C *c)
{
    PMF *p = c->pp;
    (c->**p)();
}

// @symbol func_ov080_0212513c
extern "C" void func_ov080_0212513c(char *c, int i)
{
    *(char **)(c + 0x36c) = data_ov080_0212847c + (i << 4);
    func_ov080_02125104((C *)c);
}

// @symbol _ZN9daBttBk_c16CleanupResourcesEv
s32 daBttBk_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov080_02128468)->Release();
    return 1;
}

// @symbol _ZN9daBttBk_c16OnPendingDestroyEv
void daBttBk_c::OnPendingDestroy()
{
}

// @symbol _ZN9daBttBk_c6RenderEv
int daBttBk_c::Render()
{
    int r1 = mFlags;
    r1 = r1 & 0x40000;
    r1 = r1 ? 1 : 0;
    if (r1) return 1;
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daBttBk_c8BehaviorEv
int daBttBk_c::Behavior()
{
    func_ov080_021250c8((char *)this);
    func_ov080_02124c3c((char *)this);
    return 1;
}

// @symbol _ZN9daBttBk_c13InitResourcesEv
int daBttBk_c::InitResources()
{
    Vector3 pos;
    void *file = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov080_02128468);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(((char *)this) + 0xd4, file, 1, 1);
    _ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(((char *)this) + 0x14c, ((char *)this), 0x64000, 0x78000, 0x800004, 0x9010);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    {
        int p60;
        pos.x = mPosX;
        p60 = mPosY;
        pos.y = p60;
        pos.z = mPosZ;
        pos.y = p60 + 0xc8000;
    }
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(((char *)this) + 0x180, ((char *)this), 0x32000, 0x32000, 0, 0);
    unk_374 = 0;
    ((Fn2513c1)func_ov080_0212513c)((char *)this);
    func_ov080_02124c3c((char *)this);
    return 1;
}

// @symbol _ZN9daBttBk_c13OnTurnIntoEggER6Player
void daBttBk_c::OnTurnIntoEgg(Player &player)
{
    if (player.IsCollectingCap()) {
        GivePlayerCoins(player, 5, 0);
    }
    Vector3 vec;
    Vector3 vec2;
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0xb4000;
    vec.x = x;
    vec.y = y;
    vec.z = z;
    ((int *)&vec2)[0] = ((int *)&vec)[0];
    ((int *)&vec2)[1] = ((int *)&vec)[1];
    ((int *)&vec2)[2] = ((int *)&vec)[2];
    PoofDustAt(vec2);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(6, vec.x, vec.y, vec.z);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}
