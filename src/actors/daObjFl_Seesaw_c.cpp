//cpp
/**
 * Lethal Lava Land seesaw. ov022/daObjFl_Seesaw_c, 7 functions,
 * .text 0x02111cac..0x02111f3c.
 *
 * daObjFl_Seesaw_c_classInit at 0x02111f3c is the next function and stays
 * in its own file.
 *
 * #pragma defer_codegen off emits .text in source order, which is ROM order.
 * One out-of-line destructor supplies D1 and D0.
 */

#include "math/Matrix.h"
#include "daObjFl_Seesaw_c.h"
#include "SharedFilePtr.h"

struct CLPS_Block;

extern "C" {
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void func_020393a4(int *p, int v);
void func_020393d4(int *p, int v);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int radius, int yOffset);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *thiz, void *f, const Matrix4x3 *m, int fix, short s, void *b);

extern int data_ov022_021145a8[];
extern int data_ov022_021145a0[];
extern CLPS_Block data_ov064_0211bacc;
}

#pragma defer_codegen off

// @symbol _ZN16daObjFl_Seesaw_cD1Ev
// @symbol _ZN16daObjFl_Seesaw_cD0Ev
daObjFl_Seesaw_c::~daObjFl_Seesaw_c()
{
}

// @symbol func_ov022_02111d48
extern "C" void func_ov022_02111d48(char *raw)
{
    daObjFl_Seesaw_c *t = (daObjFl_Seesaw_c *)raw;
    Matrix4x3_FromRotationXYZExt(&t->mModel.mat4x3, t->mAngleX, t->mAngleY, t->mAngleZ);
    t->mModel.mat4x3.t.x = t->mPosX >> 3;
    t->mModel.mat4x3.t.y = t->mPosY >> 3;
    t->mModel.mat4x3.t.z = t->mPosZ >> 3;
}

// @symbol _ZN16daObjFl_Seesaw_c16CleanupResourcesEv
s32 daObjFl_Seesaw_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    ((SharedFilePtr *)data_ov022_021145a8)->Release();
    ((SharedFilePtr *)data_ov022_021145a0)->Release();
    return 1;
}

// @symbol _ZN16daObjFl_Seesaw_c6RenderEv
s32 daObjFl_Seesaw_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjFl_Seesaw_c8BehaviorEv
s32 daObjFl_Seesaw_c::Behavior()
{
    func_020393a4((int *)&mMeshCollider, 0x650000);
    if (DecIfAbove0_Byte(&mSwingCooldown) == 0) {
        mAngleX += mSwingStep;
        if (mAngleX >= 0x400 || mAngleX <= -0x400) {
            mSwingStep = -mSwingStep;
            mSwingCooldown = 0x1e;
        }
    }
    func_ov022_02111d48((char *)this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0) {
        UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN16daObjFl_Seesaw_c13InitResourcesEv
s32 daObjFl_Seesaw_c::InitResources()
{
    void *f = Model::LoadFile(*(SharedFilePtr *)data_ov022_021145a8);
    mModel.SetFile((BMD_File *)f, 1, -1);
    func_ov022_02111d48((char *)this);
    UpdateClsnPosAndRot();
    void *k = dBgW_Kc::LoadFile(*(SharedFilePtr *)data_ov022_021145a0);
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, k, &mClsnMat, 0x1000, mAngleY, &data_ov064_0211bacc);
    func_020393d4((int *)&mMeshCollider, (int)&dBgW::UpdatePosWithTransform);
    mSwingStep = -0x10;
    return 1;
}
