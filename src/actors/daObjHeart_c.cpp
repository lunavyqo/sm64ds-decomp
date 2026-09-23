//cpp
/* daObjHeart_c -- the healing heart pickup. */

#pragma defer_codegen off

#include "decl_common.h"
#include "daObjHeart_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"

extern "C" {
void Matrix4x3_FromRotationY(void *m, int angleY);
unsigned char DecIfAbove0_Byte(unsigned char *p);
int Math_Function_0203b14c(int *ptr, int target, int rate, int limit, int step);

extern SharedFilePtr data_ov002_0210e104;
extern SharedFilePtr data_ov002_0210e0fc;

void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
    ModelAnim *self, BCA_File *file, int flags, int speed, unsigned short start);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    dCcAc_c *self, dActor_c *actor, int radius, int height,
    unsigned int flags, unsigned int vuln);
}

// @symbol _ZN12daObjHeart_cD1Ev
daObjHeart_c::~daObjHeart_c()
{
}

// @symbol func_ov002_020bc664
extern "C" void func_ov002_020bc664(void *raw)
{
    daObjHeart_c *heart = (daObjHeart_c *)raw;
    Matrix4x3_FromRotationY(&heart->mModelAnim.mat4x3, heart->mAngleY);
    heart->mModelAnim.mat4x3.m[9] = heart->mPosX >> 3;
    heart->mModelAnim.mat4x3.m[10] = heart->mPosY >> 3;
    heart->mModelAnim.mat4x3.m[11] = heart->mPosZ >> 3;
}

// @symbol _ZN12daObjHeart_c16CleanupResourcesEv
s32 daObjHeart_c::CleanupResources()
{
    data_ov002_0210e104.Release();
    data_ov002_0210e0fc.Release();
    return 1;
}

// @symbol _ZN12daObjHeart_c6RenderEv
s32 daObjHeart_c::Render()
{
    mModelAnim.Render(0);
    return 1;
}

// @symbol _ZN12daObjHeart_c8BehaviorEv
s32 daObjHeart_c::Behavior()
{
    s32 spin = 0x1000;
    DecIfAbove0_Byte(&mHealTimer);

    if (mdCcAc_c.otherOwner != 0) {
        if (mWasTouched == 0) {
            mWasTouched = 1;
            mHealTimer = 0;
        }
        if (mHealTimer == 0) {
            dActor_c *other = dActor_c::FindWithID(mdCcAc_c.otherOwner);
            if (other != 0) {
                int isPlayer = (int)(other->actorID == 0xbf);
                if (isPlayer != 0) {
                    s32 scaled = other->mHorzSpeed / 0x1000;
                    s32 amount = 0x100;
                    if (scaled >= 8) {
                        scaled -= 8;
                        amount = (scaled / 4 + 1) << 8;
                    }
                    ((Player *)other)->Heal(amount);
                    Sound::PlayBank3(0x61, *(Vector3 *)&mCamSpacePosX);
                }
            }
            mHealTimer = 0x5a;
        }
    } else {
        mWasTouched = 0;
    }

    if (mHealTimer > 0x2d)
        spin = 0x8000;
    Math_Function_0203b14c(&mAnimSpeed, spin, 0x400, 0x2000, 0x80);
    mModelAnim.speed = mAnimSpeed;
    mModelAnim.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN12daObjHeart_c13InitResourcesEv
s32 daObjHeart_c::InitResources()
{
    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov002_0210e104), 1, -1);
    /* Leftover: InitResources. ModelAnim::SetAnim. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, (BCA_File *)Animation::LoadFile(data_ov002_0210e0fc),
        0, 0x1000, 0);
    /* Leftover: InitResources. dCcAc_c::Init. */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x3c000, 0x78000, 0x100002, 0x8000);
    mAnimSpeed = 0x1000;
    func_ov002_020bc664(this);
    return 1;
}

// @symbol daObjHeart_c_classInit
extern "C" daObjHeart_c *daObjHeart_c_classInit(void)
{
    return new daObjHeart_c();
}
