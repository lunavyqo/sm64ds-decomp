//cpp
/* Fire piranha (FIREPAKUN / FIREPAKUN_S / PAKUN2), ov084 0x0212e554..0x0212ea18.
 *
 * The vtable address point at 0x02130b28 is preceded by a pointer to
 * _ZTI8daFPkn_c (0x02130ac0). That typeinfo names the string "8daFPkn_c"
 * at 0x02130ab4 and the base _ZTI12dEnemyBase_c. These seven methods are
 * the contiguous _ZN8daFPkn_c* run. The deleting destructor
 * sits earlier, past the state helpers, and the three classInit factories
 * start at 0x0212ea18.
 *
 * Functions are in ROM order under `#pragma defer_codegen off`.
 */

#include "decl_common.h"
#include "daFPkn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"

#pragma defer_codegen off

/* Second word is the loaded BCA. Same two words as daPkn_c's PknSharedFile. */
struct PknSharedFile {
    int id;
    void *file;
};
typedef char PknSharedFile_size_must_be_0x8[sizeof(PknSharedFile) == 0x8 ? 1 : -1];

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned short cc);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *v, int r, int h, unsigned int e, unsigned int g);
void LoadBlueCoinModel(void *c);
void UnloadBlueCoinModel(void *);
extern SharedFilePtr data_ov084_02130dfc;
extern SharedFilePtr *data_ov084_021302f4[];
extern SharedFilePtr data_ov002_0210da38;
extern PknSharedFile data_ov084_02130df4;
}

// @symbol _ZN8daFPkn_c16CleanupResourcesEv
int daFPkn_c::CleanupResources()
{
    data_ov084_02130dfc.Release();
    for (int i = 0; i < 6; i++)
        data_ov084_021302f4[i]->Release();
    data_ov002_0210da38.Release();
    UnloadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN8daFPkn_c6RenderEv
/* Leftover: Render. Folding the scale-zero test and the 0x40000 flag
   into one condition DIFFs; the flag stays in its own temporary. */
int daFPkn_c::Render()
{
    int v = mScale;
    int b;
    if (v == 0 || (b = (mFlags & 0x40000) != 0, b != 0)) {
        return 1;
    }
    Vector3 s;
    s.x = v;
    s.y = v;
    s.z = v;
    mModelAnim.Render(&s);
    return 1;
}

// @symbol _ZN8daFPkn_c8BehaviorEv
int daFPkn_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);
    int b = (mFlags & 0x60000) != 0;
    if (b != 0) {
        func_ov084_0212d564(((char *)this));
        return 1;
    }
    mModelAnim.Advance();
    int s = mState;
    switch (s) {
    case 0:
        func_ov084_0212e4e0(((char *)this));
        break;
    case 1:
        func_ov084_0212e010(((char *)this));
        break;
    case 2:
        func_ov084_0212ddbc(((char *)this));
        break;
    case 3:
        func_ov084_0212dc30(((char *)this));
        break;
    case 4:
        break;
    }
    /* Leftover: Behavior. mStateTimer is s16, so ++ sign-extends (ldrsh).
       The ROM increments the halfword unsigned. */
    {
        unsigned short *p = (unsigned short *)&mStateTimer;
        *p = *p + 1;
        if (s != mState)
            *p = 0;
    }
    func_ov084_0212d86c(((char *)this));
    func_ov084_0212d564(((char *)this));
    mdCcAc_c.Clear();
    mdCcAc_c.radius = mScale * mClsnRadiusFactor;
    mdCcAc_c.height = mScale * mClsnHeightFactor;
    mdCcAc_c.Update();
    mdCcAcPos_c.Clear();
    int b2 = actorID == 0xfc;
    if (b2 == 0
        && (unsigned int)(mState - 2) <= 1
        && mScale == mMaxScale) {
        mdCcAcPos_c.SetPosRelativeToActor(mClsnOffset);
        mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN8daFPkn_c13InitResourcesEv
int daFPkn_c::InitResources()
{
    int i;
    Vector3 v;
    int id;
    int cond;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov084_02130dfc), 1, -1);

    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);

    Model::LoadFile(data_ov002_0210da38);
    LoadBlueCoinModel(this);

    /* Leftover: InitResources. ModelAnim::SetAnim and the two dCc Init
       calls take Fix12<int> by value; spelling them that way DIFFs, so the
       calls stay the scalar externs. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov084_02130df4.file, 0x40000000, 0x1000, 0);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0, 0, 0x200001, 0x66fe0);

    v.x = 0;
    v.y = 0;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x4b000, 0x64000, 0x200002, 0x66fe0);

    mScale = 0;
    mRespawnMode = 0;
    mState = 0;
    mGroupLeaderID = 0;
    unk_1f4 = 0;
    mGroupAliveCount = 0;
    mGroupDefeatedCount = 0;
    unk_21c = 0;
    unk_21d = 0;
    mSuppressDeathReward = 1;
    unk_228 = 0;
    unk_224 = unk_228;

    id = actorID;
    cond = (id == 0xfc);
    if (cond != 0) {
        mClsnRadiusFactor = 0x3c;
        mClsnHeightFactor = 0xaa;
        mMaxScale = 0x800;
        mScaleRate = 0x52;
        mState = 1;
        mdCcAc_c.vulnFlags |= 0x8000;
    } else {
        cond = (id == 0xfd);
        if (cond != 0) {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0xaa;
            mMaxScale = 0x1000;
            mScaleRate = 0xa4;
            mState = 1;
        } else {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0x96;
            mMaxScale = 0x2000;
            mScaleRate = 0x147;
            mdCcAcPos_c.radius = 0x64000;
            mdCcAcPos_c.height = 0x64000;
            if (GetBitInDeathTable() != 0)
                mAlive = 0;
            else
                mAlive = 1;
        }
    }

    mStarID = (unsigned char)(param1 & 0xf);
    return 1;
}

// @symbol _ZN8daFPkn_c16OnAimedAtWithEggEv
s32 daFPkn_c::OnAimedAtWithEgg() {
    if (mdCcAc_c.flags & 1)
        return mScale * 100;
    int reach = mScale * mClsnHeightFactor >> 1;
    int floor = mScale * 100;
    if (reach <= floor)
        reach = floor;
    return reach;
}

// @symbol _ZN8daFPkn_c13OnTurnIntoEggER6Player
void daFPkn_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, 1, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daFPkn_c13OnYoshiTryEatEv
/* Leftover: OnYoshiTryEat keeps the two-step flag. A single
   `return actorID == 0xfc ? 4 : 0` DIFFs. */
s32 daFPkn_c::OnYoshiTryEat() {
    int r;
    if (actorID == 0xfc)
        r = 1;
    else
        r = 0;
    if (r != 0)
        r = 4;
    else
        r = 0;
    return r;
}
