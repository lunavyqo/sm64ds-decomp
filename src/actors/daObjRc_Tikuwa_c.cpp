//cpp
/* daObjRc_Tikuwa_c -- the donut block that falls.
 * ov036 0x0211193c..0x02111ca4. */

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjRc_Tikuwa_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
void dBgCh_Actr_UpdateContinuous_Veneer(dBgCh_Actr *clsn);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, Matrix4x3 *mat, int scale, s16 angY, void *clps);
void func_020393d4(dBgW *bgw, void *fn);
void func_020393c4(dBgW *bgw, void *fn);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    dBgCh_Actr *clsn, dActor_c *actor, int radius, int height, void *a, void *b);
}

// @symbol _ZN16daObjRc_Tikuwa_cD1Ev
// @symbol _ZN16daObjRc_Tikuwa_cD0Ev
daObjRc_Tikuwa_c::~daObjRc_Tikuwa_c()
{
}

// @symbol _ZN16daObjRc_Tikuwa_c16CleanupResourcesEv
s32 daObjRc_Tikuwa_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    ((SharedFilePtr *)data_ov036_0211408c)->Release();
    ((SharedFilePtr *)data_ov036_02114084)->Release();
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c16OnPendingDestroyEv
void daObjRc_Tikuwa_c::OnPendingDestroy()
{
}

// @symbol _ZN16daObjRc_Tikuwa_c6RenderEv
s32 daObjRc_Tikuwa_c::Render()
{
    if (mState != 2)
        mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c8BehaviorEv
s32 daObjRc_Tikuwa_c::Behavior()
{
    switch (mState) {
    case 0:
        if (mHadClsn == 0)
            mClsnTimer = 0;
        else {
            mClsnTimer += 1;
            mHadClsn = 0;
        }
        if (mClsnTimer >= 0xf)
            mState = 1;
        break;
    case 1:
        UpdatePos(0);
        dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
        if (mWithMeshClsn.IsOnGround() == 0) {
            if (DistToCPlayer() <= 0x9c4000)
                break;
        }
        TriplePoofDust();
        if (mMeshCollider.IsEnabled() != 0)
            mMeshCollider.Disable();
        mPosX = mHomePosX;
        mPosY = mHomePosY;
        mPosZ = mHomePosZ;
        mState = 2;
        break;
    case 2: {
        s32 dist = DistToCPlayer();
        if (dist <= 0x3e8000)
            break;
        if (dist < 0x7d0000) {
            mVertSpeed = 0;
            mClsnTimer = 0;
            mHadClsn = 0;
            mState = 0;
        }
        break;
    }
    }
    UpdateModelPosAndRotY();
    if (mState != 2) {
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x5dc000, 0) != 0)
            UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN16daObjRc_Tikuwa_c13InitResourcesEv
s32 daObjRc_Tikuwa_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)data_ov036_0211408c), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    void *kcl = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov036_02114084);
    /* Leftover: InitResources. SetFile's method takes Fix12<int> and a
     * CLPS_Block &; that call does not compile here. Init's header takes
     * Fix12i and mangles a symbol the ROM does not have. */
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, kcl, &mClsnMat, 0x1000, mAngleY, data_ov036_02112b48);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithVelocity);
    func_020393c4(&mMeshCollider, (void *)func_ov036_02111cc4);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x64000, 0, 0);
    mTerminalVelocity = -0x1e000;
    mVertAccel = ~0x198;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    return 1;
}
