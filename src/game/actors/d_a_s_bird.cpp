//cpp
/* Castle grounds birds -- ov009/daSBird_c, profile SBIRD (BIRD 343).
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113928 names daSBird_c; the debug table names SBIRD.
 * This is the flock, not the flag and not castle water.
 *
 * daSBird_c_classInit is reconstructed (RTTI daSBird_c, SBIRD registry).
 * Retail does not store that spelling. Historical alias: Bird_Spawn.
 *
 * Factory is `return new daSBird_c()`. fBase_c::operator new(unsigned long)
 * forwards `_ZN7fBase_cnwEj`. The inline destructor in the header emits the
 * required D1/D0 pair; `new` odr-uses the class so that pair is emitted here.
 *
 * deslop leftovers:
 * - ModelAnim::SetAnim / DropShadowRadHeight 6az: this TU passes Fix12<int>
 *   by value; the header method form size-DIFFs.
 * - cstd::atan2 6az (Fix12<int> by value) in func_ov009_02111234.
 * - Vec3_Asr / Vec3_Sub / Vec3_HorzLen / LenVec3 / ApproachLinear: no shared
 *   header this TU can take without a campaign.
 * - data_ov009_02113c20 / 02113c28 SharedFilePtr handles: sinit constructs
 *   them as file IDs 1080 / 1081; overlay .bss owns them. symbols.txt has
 *   no recovered names, so they are not coined.
 * - data_ov009_02113c48 is the 4-state PMF table this Behavior indexes;
 *   overlay .bss owns it. Helpers stay func_ov009_* (table order: 021116ec,
 *   021115d8, 0211145c, 02111234). 02111224 is the follower-attach called
 *   from the two spawn loops.
 * - func_ov009_0211145c is a proven mwccarm 1.2/2004/b56 register-
 *   allocation wall (outgoing 0x6a steals r0 from the 0xff06a000 pool
 *   constant). Byte-exact via an asm hatch; does not count as matched C.
 * - *(Vector3 *)&mPosX addressing shape (a Vector3 member at 0x05c is a
 *   dActor_c campaign, not this leaf).
 * - data_020a0e68 scratch matrix; data_02082214 sine table;
 *   data_0209e650 RNG; func_0201267c (sound 0x6a at mCamSpacePos).
 * - RandomIntInternal stays unsigned: signed % 400 changed codegen.
 * - S14: g_profile_SBIRD stays outside the licensed .text.
 * - common.h for Vector3_16 (Spawn rotation); included after the class
 *   header so ModelAnim's Matrix4x3 spelling wins.
 */

#include "daSBird_c.h"
#include "common.h"
#include "SharedFilePtr.h"

typedef void (daSBird_c::*BirdState)();

struct BirdMtx {
    int w[12];
};

extern "C" {
extern SharedFilePtr data_ov009_02113c20;
extern SharedFilePtr data_ov009_02113c28;
extern BirdState data_ov009_02113c48[];
extern BirdMtx data_020a0e68;
extern s16 data_02082214[];
extern unsigned int RandomIntInternal(void *rng);
extern int data_0209e650;
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Vec3_Sub(void *out, void *a, void *b);
extern s32 Vec3_HorzLen(void *v);
extern s32 LenVec3(void *v);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationZ(void *m, s16 a);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 a);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
extern void _Z14ApproachLinearRsss(void *p, s16 target, s16 step);
extern int func_0201267c(unsigned int a, void *b);
extern void func_ov009_02111224(char *c, int r1);
extern dActor_c *_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
    unsigned int id, unsigned int p1, const Vector3 *pos, const Vector3_16 *rot,
    int a, int b);
extern dActor_c *_ZN8dActor_c10FindWithIDEj(u32 id);
extern Player *_ZN8dActor_c13ClosestPlayerEv(void *self);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *self, void *cc);
extern void _ZN7fBase_c18MarkForDestructionEv(void *self);

/* ModelAnim::SetAnim -- wall 6az. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *animFile, int flags, int speed, u32 startFrame);

/* dActor_c::DropShadowRadHeight -- wall 6az. */
int _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    dActor_c *self, ShadowModel *sm, Matrix4x3 *m, int rad, int h, unsigned u);
}

extern int _ZTV9daSBird_c[];

// @symbol daSBird_c_classInit
extern "C" daSBird_c *daSBird_c_classInit()
{
    return new daSBird_c();
}

// @symbol _ZN9daSBird_c13InitResourcesEv
s32 daSBird_c::InitResources()
{
    BMD_File *modelFile = (BMD_File *)Model::LoadFile(data_ov009_02113c20);
    mModelAnim.SetFile(modelFile, 1, 1);
    BCA_File *animFile = (BCA_File *)Animation::LoadFile(data_ov009_02113c28);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, animFile, 0, 0x1000, 0);
    mShadowModel.InitCylinder();
    {
        int *p60 = &mPosY;
        int y = *p60;
        int zero = 0;
        *p60 = y + 0xa000;
        mVertAccel = zero;
        mTerminalVelocity = -0x32000;
        mIsLeader = 1;
        mOwnerID = uniqueID;
        mTargetPos.x = mPosX;
        mTargetPos.y = mPosY;
        mTargetPos.z = mPosZ;
        mState = zero;
    }
    return 1;
}

// @symbol _ZN9daSBird_c8BehaviorEv
s32 daSBird_c::Behavior()
{
    (this->*data_ov009_02113c48[mState])();
    int tmp[3];
    Vec3_Asr(tmp, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, tmp[0], tmp[1], tmp[2]);
    mAngleY = mPrevAngleY;
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mAngleZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    *(BirdMtx *)((char *)&mModelAnim.mat4x3) = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mModelAnim.mat4x3, 0x1e000, 0x7d0000, 0xf);
    mModelAnim.Advance();
    return 1;
}

// @symbol _ZN9daSBird_c6RenderEv
s32 daSBird_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN9daSBird_c16OnPendingDestroyEv
void daSBird_c::OnPendingDestroy()
{
}

// @symbol _ZN9daSBird_c16CleanupResourcesEv
s32 daSBird_c::CleanupResources()
{
    data_ov009_02113c28.Release();
    data_ov009_02113c20.Release();
    return 1;
}

// @symbol func_ov009_021116ec
extern "C" void func_ov009_021116ec(char *c)
{
    if (*(unsigned char *)(c + 0x180)) {
        int n = *(int *)(c + 8) & 0xf;
        if (n > 1) {
            Vector3_16 rot = *(Vector3_16 *)(c + 0x92);
            for (int i = 0; i < n - 1; i++) {
                Vector3 pos;
                pos.x = *(int *)(c + 0x5c)
                    + (int)((RandomIntInternal(&data_0209e650) % 400) - 0xa0)
                        * 4096;
                pos.y = *(int *)(c + 0x60);
                pos.z = *(int *)(c + 0x64)
                    + (int)((RandomIntInternal(&data_0209e650) % 400) - 0xa0)
                        * 4096;
                rot.y = (short)(rot.y
                    + (short)(RandomIntInternal(&data_0209e650) >> 16));
                void *a = _ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                    0x157, 0, &pos, &rot, (int)*(signed char *)(c + 0xcc), -1);
                if (a)
                    func_ov009_02111224((char *)a, *(int *)(c + 4));
            }
        }
        *(int *)(c + 0x160) = -0x14000;
        *(int *)(c + 0x168) = 0xff06a000;
    }
    *(int *)(c + 0x17c) = 1;
}

// @symbol func_ov009_021115d8
extern "C" void func_ov009_021115d8(char *c)
{
    if (*(unsigned char *)(c + 0x180) != 0) {
        char *p2 = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
        if (p2 != 0) {
            struct Vector3 copy;
            struct Vector3 diff;
            int len;
            Vec3_Sub(&diff, c + 0x5c, p2 + 0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            len = Vec3_HorzLen(&copy);
            if (len > 0x7d0000)
                return;
        }
        func_0201267c(0x6a, c + 0x74);
    } else {
        char *p = (char *)_ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x178));
        if (p == 0) {
            _ZN7fBase_c18MarkForDestructionEv(c);
            return;
        }
        if (*(int *)(p + 0x17c) != 3)
            return;
    }
    *(short *)(c + 0x92) = 5000
        - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(int *)(c + 0x174) = 0x28000;
    *(int *)(c + 0x17c) = 3;
    *(int *)(((int)c + 0xb0)) &= ~0x10000;
}

// @symbol func_ov009_0211145c
/* HAND-WRITTEN ASM, not a C decompilation. Byte-exact via an asm hatch on a
 * proven mwccarm register-allocation/scheduling wall. The bytes reproduce, but
 * this member is not decompiled -- it should not be read as matched C, and it
 * is a draft until someone reproduces the bytes from real C.
 *
 * Store/call tail: pure C emits rsb r0 + wrong str/mov interleave for
 * -0x14000 / 0xff06a000 / func_0201267c. The outgoing 0x6a pins r0 before
 * the 0x168 store is scheduled; sibling 021116ec colours the same constant
 * r0 only because it has no call after the stores. */
extern "C" void func_ov009_0211145c(char *c)
{
    if (*(unsigned char *)(c + 0x180) != 0) {
        char *p;
        struct Vector3 copy;
        struct Vector3 diff;
        int n;
        p = (char *)_ZN8dActor_c13ClosestPlayerEv(c);
        if (p == 0) {
            return;
        }
        Vec3_Sub(&diff, c + 0x5c, p + 0x5c);
        copy.x = diff.x;
        copy.y = diff.y;
        copy.z = diff.z;
        if (Vec3_HorzLen(&copy) > 0x7d0000) {
            return;
        }
        n = *(int *)(c + 8) & 0xf;
        if (n > 1) {
            int i = 0;
            int lim = n - 1;
            if (lim > 0) {
                do {
                    char *a = (char *)_ZN8dActor_c5SpawnEjjRK7Vector3PK10Vector3_16as(
                        0x157, 0x10, (Vector3 *)(c + 0x5c), 0,
                        *(signed char *)(c + 0xcc), -1);
                    if (a != 0) {
                        func_ov009_02111224(a, *(int *)(c + 4));
                    }
                    i++;
                } while (i < lim);
            }
        }
        asm {
            mov r0,#0x14000
            rsb r1,r0,#0
            str r1,[c,#0x160]
            ldr r0,=0xff06a000
            add r1,c,#0x74
            str r0,[c,#0x168]
            mov r0,#0x6a
            bl func_0201267c
        }
    }
    *(short *)(c + 0x92) = 5000
        - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(unsigned short *)(c + 0x94) =
        (unsigned int)RandomIntInternal(&data_0209e650) >> 16;
    *(int *)(c + 0x174) = 0x28000;
    *(int *)(c + 0x17c) = 3;
    *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) &= ~0x10000;
}

// @symbol func_ov009_02111234
extern "C" void func_ov009_02111234(void *self)
{
    u8 *c = (u8 *)self;
    _ZN8dActor_c9UpdatePosEP5dCc_c(self, 0);
    u8 *f = (u8 *)_ZN8dActor_c10FindWithIDEj(*(u32 *)(c + 0x178));
    if (f == 0 || *(s32 *)(f + 0x60) > 0xbb8000) {
        _ZN7fBase_c18MarkForDestructionEv(self);
        return;
    }

    Vector3 diff;
    if (*(u8 *)(c + 0x180) != 0) {
        Vector3 d1;
        Vec3_Sub(&d1, (void *)(c + 0x160), (void *)(c + 0x5c));
        diff.x = d1.x;
        diff.y = d1.y;
        diff.z = d1.z;
        s32 hl = Vec3_HorzLen(&diff);
        *(s16 *)(c + 0x16c) =
            _ZN4cstd5atan2E5Fix12IiES1_(hl, *(s32 *)(c + 0x60) + 0xfd8f0000);
        *(s16 *)(c + 0x16e) =
            _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
    } else {
        Vector3 d2;
        Vec3_Sub(&d2, (void *)(f + 0x5c), (void *)(c + 0x5c));
        diff.x = d2.x;
        diff.y = d2.y;
        diff.z = d2.z;
        s32 hl = Vec3_HorzLen(&diff);
        *(s16 *)(c + 0x16c) =
            _ZN4cstd5atan2E5Fix12IiES1_(hl, -diff.y);
        *(s16 *)(c + 0x16e) =
            _ZN4cstd5atan2E5Fix12IiES1_(diff.z, diff.x);
        *(s32 *)(c + 0x174) = LenVec3(&diff) / 25 + 0x14000;
    }

    _Z14ApproachLinearRsss((void *)(c + 0x92), *(s16 *)(c + 0x16c), 0x8c);
    _Z14ApproachLinearRsss((void *)(c + 0x94), *(s16 *)(c + 0x16e), 0x320);

    s32 hi = 0x3000;
    s32 dd = (s16)(*(s16 *)(c + 0x94) - *(s16 *)(c + 0x16e));
    if (dd < -hi)
        dd = -hi;
    else if (dd > hi)
        dd = hi;
    *(s16 *)(c + 0x170) = (s16)dd;
    _Z14ApproachLinearRsss((void *)(c + 0x90), *(s16 *)(c + 0x170), 0x258);

    {
        u16 a = *(u16 *)(c + 0x92);
        s32 t = *(s32 *)(c + 0x174);
        s16 s = data_02082214[((a >> 4) << 1) + 1];
        *(s32 *)(c + 0x98) = (s32)(((s64)t * s + 0x800) >> 12);
    }
    {
        u16 a = *(u16 *)(c + 0x92);
        s32 t = *(s32 *)(c + 0x174);
        s16 s = data_02082214[(a >> 4) << 1];
        *(s32 *)(c + 0xa8) = (s32)(((s64)t * s + 0x800) >> 12);
    }
}

// @symbol func_ov009_02111224
extern "C" void func_ov009_02111224(char *c, int r1)
{
    c[0x180] = 0;
    *(int *)(c + 0x178) = r1;
}
