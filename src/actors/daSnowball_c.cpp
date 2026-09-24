//cpp
/* ov081 daSnowball_c methods, .text 0x02126298..0x021264b4.
 *
 * Five complete delinks, abutting, nothing between them. D1/D0 sit at
 * 0x02125f14..0x02125fb8, on the far side of other functions, and the
 * factory daSnowball_c_classInit starts at 0x021264b4. Neither is in
 * this run.
 *
 * Vtable 0x02128abc: the word at -4 is _ZTI12daSnowball_c (0x02128a7c).
 * The ROM string at 0x02128a88 is "12daSnowball_c". The tree used to
 * call this class Snowball; that spelling is not in the cartridge.
 *
 * decl_common.h is first so common.h's Matrix4x3 wins over math/Matrix.h.
 * Source order is ROM order under defer_codegen off.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daSnowball_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov081_02128d90;

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *thiz, void *actor, int radius, int height, unsigned int flags, unsigned int vuln);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *thiz, void *actor, int radius, int height, void *a, int b);
}

/* Behavior's state call. The pointer lives in unk_378; the member
 * function sits 8 bytes into that record. */
struct SnowballState;
typedef void (SnowballState::*SnowballStateFn)();
struct SnowballStateRec { char pad[8]; SnowballStateFn fn; };

// @symbol _ZN12daSnowball_c16CleanupResourcesEv
int daSnowball_c::CleanupResources()
{
    data_ov081_02128d90.Release();
    return 1;
}

// @symbol _ZN12daSnowball_c16OnPendingDestroyEv
void daSnowball_c::OnPendingDestroy()
{
}

// @symbol _ZN12daSnowball_c6RenderEv
int daSnowball_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN12daSnowball_c8BehaviorEv
int daSnowball_c::Behavior()
{
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    SnowballStateRec *rec = *(SnowballStateRec **)&unk_378;
    if (rec->fn != 0)
        (((SnowballState *)(char *)this)->*(rec->fn))();
    int v = mVertSpeed + mVertAccel;
    int hi = mTerminalVelocity;
    if (v >= hi)
        hi = v;
    int tmp = unk_0ac;
    mVertSpeed = hi;
    unk_0ac = tmp;
    UpdatePosWithOnlySpeed((dCc_c *)&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleY = mPrevAngleY;
    func_ov081_02126224((char *)this);
    mdCcAc_c.Clear();
    Player *player = ClosestPlayer();
    if (player != 0 && player->mIsVanish == 0)
        mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN12daSnowball_c13InitResourcesEv
int daSnowball_c::InitResources()
{
    void *file = Model::LoadFile(data_ov081_02128d90);
    if (mModel.SetFile((BMD_File *)file, 1, -1) == 0)
        return 0;

    mShadowModel.InitCylinder();

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    /* Leftover: InitResources. dCcAc_c::Init(Fix12<int> by value) spills the
       radii into .rodata and the function no longer matches. dBgCh_Actr::Init
       in the header is Fix12i, which is not the ROM symbol (5Fix12IiE). */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x1e000, 0x1e000, 0x200004, 0x40010);

    unk_37c = mPosX;
    unk_380 = mPosY;
    unk_384 = mPosZ;
    mPosY += 0x32000;
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x14000, 0x14000, 0, 0);

    func_ov081_021261d4(this, &data_ov081_02128eb4);
    return 1;
}
