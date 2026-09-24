//cpp
/**
 * The seven consecutive daObjBlockS_c methods at 0x02139a50..0x02139f04.
 * D1, D0 and Kill sit earlier, with other complete functions between
 * them, so they stay in their own delinks. daObjBlockS_c_classInit is
 * the next function and is not part of this run.
 *
 * Emission is ROM-ascending under `#pragma defer_codegen off`. This TU
 * does not define ~daObjBlockS_c: that definition lives in the earlier D1/D0
 * shards, and a second one would emit another destructor pair.
 */

#include "decl_Actor.h"
#include "decl_common.h"
#include "daObjBlockS_c.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct Vector3_16f;

extern "C" {
extern u32 data_0209b454;
extern int data_ov098_0213c4c8[];
extern void _ZN6Player9DropActorEv(void *self);
extern int Vec3_HorzDist(const struct Vector3 *a, const struct Vector3 *b);
extern int _ZN8dActor_c13DistToCPlayerEv(void *self);
extern void Crate_SetState(char *c, int i);
extern u8 DecIfAbove0_Byte(u8 *p);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, int c, int d, int e, const void *v, void *cb);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
    u32 a, u32 b, int c, int d, int e, const Vector3_16f *v);
extern int _ZN5Model8LoadFileER13SharedFilePtr(char *f);
extern void _ZN9ModelBase7SetFileEP8BMD_Fileii(char *thiz, int f, int a, int b);
extern void _ZN11ShadowModel10InitCuboidEv(char *thiz);
extern int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(char *f);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    char *thiz, int f, char *m, int fix, short s, int blk);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    char *thiz, char *actor, int b, int d, void *v, int f);
extern void _ZN10dBgCh_Actr19StartDetectingWaterEv(char *thiz);
}

enum Bool { FALSE, TRUE };

struct Sub {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void m5(int);
};

#pragma defer_codegen off

// @symbol _ZN13daObjBlockS_c16CleanupResourcesEv
int daObjBlockS_c::CleanupResources()
{
    int *f;
    if (((dBgW *)((char *)&mMeshCollider))->IsEnabled())
        ((dBgW *)((char *)&mMeshCollider))->Disable();
    f = 0;
    if (actorID == 0xc2)
        f = data_ov098_0213c4c8;
    if (f) {
        ((SharedFilePtr *)((void *)f[0]))->Release();
        ((SharedFilePtr *)((void *)f[1]))->Release();
    }
    return 1;
}

// @symbol _ZN13daObjBlockS_c6RenderEv
int daObjBlockS_c::Render()
{
    if (mState == 6)
        return 1;
    {
        int b = (int)((mFlags & 0x40000) != 0);
        if (b)
            return 1;
    }
    Sub *o = (Sub *)&mModel;
    o->m5(0);
    return 1;
}

// @symbol _ZN13daObjBlockS_c8BehaviorEv
int daObjBlockS_c::Behavior()
{
    struct Vector3 v;
    struct Vector3 vec;
    struct Vector3 t;
    struct Vector3 vec2;
    enum Bool b1, b2;
    int x, y, z;

    b1 = (enum Bool)((mFlags & 0x4000000) != 0);
    if (b1 != FALSE && (data_0209b454 & 0x4000000) && mHoldingPlayer) {
        _ZN6Player9DropActorEv(mHoldingPlayer);
    }

    b2 = (enum Bool)((mFlags & 8) != 0);
    if (b2 != FALSE
        && Vec3_HorzDist((struct Vector3 *)&mPosX, (const struct Vector3 *)&mHomePosX)
        && _ZN8dActor_c13DistToCPlayerEv(((char *)this)) > 0x7d0000) {
        Crate_SetState(((char *)this), 6);
        return 1;
    }

    if (mBreakTimer != 0) {
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x50000;
        ((int *)&v)[0] = x;
        ((int *)&v)[1] = y;
        ((int *)&v)[2] = z;
        if (DecIfAbove0_Byte(&mBreakTimer)) {
            mParticleHandle1 = (u32)_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
                mParticleHandle1, 0x13a, v.x, v.y, v.z, 0, 0);
            mParticleHandle2 = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
                mParticleHandle2, 0x13b, v.x, v.y, v.z, 0);
            goto done;
        }
        func_ov098_02138e08(((char *)this));
        x = mPosX;
        z = mPosZ;
        y = mPosY + 0x28000;
        vec.x = x;
        vec.y = y;
        vec.z = z;
        ((int *)&vec2)[0] = ((int *)&vec)[0];
        ((int *)&vec2)[1] = ((int *)&vec)[1];
        ((int *)&vec2)[2] = ((int *)&vec)[2];
        _ZN8dActor_c19DisappearPoofDustAtERK7Vector3(((char *)this), &vec2);
        Crate_SetState(((char *)this), 6);
        return 1;
    }

done:
    ((int *)&t)[0] = mPosX;
    ((int *)&t)[1] = mPosY;
    ((int *)&t)[2] = mPosZ;
    ((int *)&t)[1] = t.y - mClsnYOffset;
    mdCcAcPos_c1.pos.x = t.x;
    mdCcAcPos_c1.pos.y = t.y;
    mdCcAcPos_c1.pos.z = t.z;
    mdCcAcPos_c2.pos.x = t.x;
    mdCcAcPos_c2.pos.y = t.y;
    mdCcAcPos_c2.pos.z = t.z;
    func_ov098_02138b70(((char *)this));
    return 1;
}

// @symbol _ZN13daObjBlockS_c13InitResourcesEv
int daObjBlockS_c::InitResources()
{
    char *f = 0;
    if (actorID == 0xc2)
        f = (char *)data_ov098_0213c4c8;
    if (f == 0)
        return 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mHomeAngleX = mAngleX;
    mHomeAngleY = mAngleY;
    mHomeAngleZ = mAngleZ;
    _ZN9ModelBase7SetFileEP8BMD_Fileii(
        (char *)&mModel, _ZN5Model8LoadFileER13SharedFilePtr(*(char **)(f)), 1, -1);
    _ZN11ShadowModel10InitCuboidEv((char *)&mShadowModel);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(*(char **)(f + 4)),
        (char *)&mClsnMat, 0x199, mAngleY, *(int *)(f + 8));
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        (char *)&mWithMeshClsn, ((char *)this), 0x28000, 0x28000, 0, 0);
    _ZN10dBgCh_Actr19StartDetectingWaterEv((char *)&mWithMeshClsn);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    Crate_SetState(((char *)this), 0);
    /* dBgActor_c's own generic 0xd0..0xd4 pad, not a daObjBlockS_c field. */
    *(s32 *)(((char *)this) + 0xd0) = 0;
    func_ov098_02138ce0(((char *)this));
    return 1;
}

// @symbol _ZN13daObjBlockS_c15OnGroundPoundedER8dActor_c
void daObjBlockS_c::OnGroundPounded(dActor_c &other)
{
    Kill();
}

// @symbol _ZN13daObjBlockS_c13OnYoshiTryEatEv
int daObjBlockS_c::OnYoshiTryEat()
{
    unsigned char v = mBreakTimer;
    if (v != 0)
        return 0;
    return 6;
}

// @symbol _ZN13daObjBlockS_c13OnTurnIntoEggER6Player
void daObjBlockS_c::OnTurnIntoEgg(Player &player)
{
    char *r4 = (char *)&player;
    if (((Player *)r4)->IsCollectingCap()) {
        if (mCoinsPaid != 1) {
            ((dActor_c *)this)->GivePlayerCoins(*(Player *)r4, 3, 0);
            mCoinsPaid = 1;
        }
    } else {
        unsigned int count = 0;
        if (mCoinsPaid != 1) {
            mCoinsPaid = 1;
            count = 3;
        }
        ((Player *)r4)->RegisterEggCoinCount(count, 0, 0);
    }
    Crate_SetState((char *)this, 6);
}
