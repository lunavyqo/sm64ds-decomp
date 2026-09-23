//cpp
/* daObjKm2_Nobiru_c is the stretching platform. It loads the pole-lift
 * model and collision mesh, then grows and shrinks the collider's
 * vertical scale. Render draws the model at that scale.
 */

#include "decl_common.h"
#include "daObjKm2_Nobiru_c.h"
#include "SharedFilePtr.h"

#pragma defer_codegen off

extern "C" {
extern SharedFilePtr PoleLift_ClsnFile;
extern SharedFilePtr PoleLift_ModelFile;
extern s32 func_0203aad0(dBgW_KcMbgSclY *);
extern void Matrix4x3_FromRotationY(Matrix4x3 *, s32);
extern void _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(dBgW_KcMbgSclY *, s32);
extern void _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbgSclY *, KCL_File *, const Matrix4x3 &, s32, s16, CLPS_Block &);
}

// @symbol _ZN17daObjKm2_Nobiru_cD1Ev
// @symbol _ZN17daObjKm2_Nobiru_cD0Ev
daObjKm2_Nobiru_c::~daObjKm2_Nobiru_c()
{
}

// @symbol _ZN17daObjKm2_Nobiru_c23UpdateColliderTransformEv
void daObjKm2_Nobiru_c::UpdateColliderTransform()
{
    Matrix4x3_FromRotationY(&mColliderTransform, mAngleY);
    mColliderTransform.m[9] = mPosX;
    mColliderTransform.m[10] = mPosY;
    mColliderTransform.m[11] = mPosZ;
}

// @symbol _ZN17daObjKm2_Nobiru_c20UpdateModelTransformEv
void daObjKm2_Nobiru_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN17daObjKm2_Nobiru_c16CleanupResourcesEv
int daObjKm2_Nobiru_c::CleanupResources()
{
    mCollider.Disable();
    PoleLift_ModelFile.Release();
    PoleLift_ClsnFile.Release();
    return 1;
}

// @symbol _ZN17daObjKm2_Nobiru_c16OnPendingDestroyEv
void daObjKm2_Nobiru_c::OnPendingDestroy()
{
}

// @symbol _ZN17daObjKm2_Nobiru_c6RenderEv
int daObjKm2_Nobiru_c::Render()
{
    Vector3 scale;
    scale.x = 0x1000;
    /* func_0203aad0: mCollider.scaleY DIFFs Render. */
    scale.y = func_0203aad0(&mCollider);
    scale.z = 0x1000;
    mModel.Render(&scale);
    return 1;
}

// @symbol _ZN17daObjKm2_Nobiru_c8BehaviorEv
int daObjKm2_Nobiru_c::Behavior()
{
    /* func_0203aad0: mCollider.scaleY DIFFs Behavior.
     * SetScaleY: the Fix12<int> method call DIFFs Behavior. */
    s32 scaleY = func_0203aad0(&mCollider);
    if (mGrowing) {
        _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(&mCollider, scaleY + 8);
        if (func_0203aad0(&mCollider) > 0x1000)
            mGrowing = 0;
    } else {
        _ZN14dBgW_KcMbgSclY9SetScaleYE5Fix12IiE(&mCollider, scaleY - 8);
        if (func_0203aad0(&mCollider) < 0x800)
            mGrowing = 1;
    }
    return 1;
}

// @symbol _ZN17daObjKm2_Nobiru_c13InitResourcesEv
int daObjKm2_Nobiru_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(PoleLift_ModelFile), 1, -1);
    UpdateModelTransform();
    UpdateColliderTransform();

    /* SetFile: the Fix12<int> method call DIFFs InitResources.
     * func_020396c0: mCollider.unk_48 = 4 DIFFs InitResources. */
    _ZN14dBgW_KcMbgSclY7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mCollider, (KCL_File *)dBgW_Kc::LoadFile(PoleLift_ClsnFile),
        mColliderTransform, 0x1000, mAngleY, *(CLPS_Block *)data_ov045_021125b0);
    func_020396c0(&mCollider, 4);
    mCollider.unk_4d = 1;
    mCollider.Enable(this);
    mGrowing = 1;
    return 1;
}
