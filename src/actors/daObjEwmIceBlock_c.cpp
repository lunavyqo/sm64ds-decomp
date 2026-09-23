//cpp
/* The big ice block that moves along a path. */

#include "decl_common.h"
#include "daObjEwmIceBlock_c.h"
#include "SharedFilePtr.h"

namespace cstd { int fdiv(int a, int b); }

extern "C" {
extern SharedFilePtr *data_ov056_02113314[];
extern int _ZNK7PathPtr8NumNodesEv(void *p);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int LenVec3(Vector3 *v);
extern void Vec3_MulScalar(Vector3 *out, Vector3 *v, int s);
extern void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, void *kcl, const Matrix4x3 *mtx, int scale, short angle,
    void *clps);
extern void func_020393d4(void *p, void *cb);
}

#pragma defer_codegen off

// @symbol _ZN18daObjEwmIceBlock_cD1Ev
// @symbol _ZN18daObjEwmIceBlock_cD0Ev
daObjEwmIceBlock_c::~daObjEwmIceBlock_c()
{
}

// @symbol _ZN18daObjEwmIceBlock_c16CleanupResourcesEv
int daObjEwmIceBlock_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    data_ov056_02113314[0]->Release();
    data_ov056_02113314[1]->Release();
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c6RenderEv
int daObjEwmIceBlock_c::Render()
{
    mModel.Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c8BehaviorEv
/* Leftover: Behavior. PathPtr::NumNodes() is scheduled ahead of the delta < 0
 * test and three words move. dBgActor_c::IsClsnInRange stays mangled (Fix12 by
 * value). */
int daObjEwmIceBlock_c::Behavior()
{
    Vector3 prev;
    Vector3 node;
    Vector3 diff;
    Vector3 scaled1;
    Vector3 scaled2;
    int looped;
    int delta;
    int len;

    mHorzSpeed = 0xa000;
    delta = mPathNodeIdx - mPathDir;
    looped = 0;

    if (mPath.Loops()) {
        if (mPathDir > 0) {
            if (delta < 0)
                delta = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1;
        } else {
            if (delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath))
                delta = looped;
        }
        mPath.GetNode(node, delta);
    } else {
        if (delta < 0 || delta >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            node.x = mPosX;
            node.y = mPosY;
            node.z = mPosZ;
        } else {
            mPath.GetNode(node, delta);
        }
    }

    mPath.GetNode(prev, mPathNodeIdx);
    Vec3_Sub(&diff, (Vector3 *)&mPosX, &prev);
    len = LenVec3(&diff);
    if (len == 0 || len <= mHorzSpeed) {
        int q = cstd::fdiv(mHorzSpeed, len);
        Vec3_MulScalar(&scaled1, &diff, q);
        SubVec3((Vector3 *)&mPosX, &scaled1, (Vector3 *)&mPosX);
        looped = 1;
    } else {
        int q = cstd::fdiv(mHorzSpeed, len);
        Vec3_MulScalar(&scaled2, &diff, q);
        SubVec3((Vector3 *)&mPosX, &scaled2, (Vector3 *)&mPosX);
    }

    if (looped) {
        mPathNodeIdx += mPathDir;
        if (mPathNodeIdx < 0) {
            if (mPath.Loops())
                mPathNodeIdx = _ZNK7PathPtr8NumNodesEv((char *)&mPath) - 1;
            else {
                mPathDir = 1;
                mPathNodeIdx += mPathDir * 2;
            }
        }
        if (mPathNodeIdx >= _ZNK7PathPtr8NumNodesEv((char *)&mPath)) {
            if (mPath.Loops())
                mPathNodeIdx = 0;
            else {
                mPathDir = -1;
                mPathNodeIdx += mPathDir * 2;
            }
        }
    }

    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0x5dc000, 0);
    return 1;
}

// @symbol _ZN18daObjEwmIceBlock_c13InitResourcesEv
/* Leftover: InitResources. dBgW_KcMbg::SetFile stays mangled (Fix12 by value).
 * func_020393d4 stores the mesh callback; there is no named setter. */
int daObjEwmIceBlock_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(*data_ov056_02113314[0]), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        dBgW_Kc::LoadFile(*data_ov056_02113314[1]),
        &mClsnMat,
        0x199,
        mAngleY,
        data_ov056_02113314[2]);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosWithTransform);
    mPath.FromID(param1 & 0xff);
    mPathDir = 1;
    mHorzSpeed = 0xa000;
    return 1;
}
