//cpp
/* daFRing_c -- Bowser's shockwaves. */

#pragma defer_codegen off

#include "decl_common.h"
#include "daFRing_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

extern "C" {
void func_02016b24(void *model, unsigned int mask);
Fix12i Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
/* InitResources: these four take Fix12<int> by value, and that spelling
 * spills the 0x1000. The int bridge is what matches. */
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim *self, BCA_File *file, int flags, int speed, unsigned int start);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(TextureSequence *self, BTP_File *file, int flags, int speed, unsigned int start);
void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(MaterialChanger *self, BMA_File *file, int flags, int speed, unsigned int start);
void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(TextureTransformer *self, BTA_File *file, int flags, int speed, unsigned int start);
}

// @symbol _ZN9daFRing_cD1Ev
// @symbol _ZN9daFRing_cD0Ev
daFRing_c::~daFRing_c()
{
}

// @symbol _ZN9daFRing_c16CleanupResourcesEv
s32 daFRing_c::CleanupResources()
{
    ((SharedFilePtr *)&data_ov060_0211b208)->Release();
    ((SharedFilePtr *)&data_ov060_0211b1f8)->Release();
    ((SharedFilePtr *)&data_ov060_0211b200)->Release();
    return 1;
}

// @symbol _ZN9daFRing_c6RenderEv
s32 daFRing_c::Render()
{
    mTextureSequence1.Update(mModelAnim1.data);
    mTextureSequence2.Update(mModelAnim2.data);
    mMaterialChanger1.Update(mModelAnim1.data);
    mMaterialChanger2.Update(mModelAnim2.data);
    mTextureTransformer1.Update(mModelAnim1.data);
    mTextureTransformer2.Update(mModelAnim2.data);
    mModelAnim1.Render(0);
    Vector3 scale;
    scale.x = 0x800;
    scale.y = 0x800;
    scale.z = 0x800;
    mModelAnim2.Render(&scale);
    return 1;
}

// @symbol _ZN9daFRing_c8BehaviorEv
s32 daFRing_c::Behavior()
{
    mFrame++;
    Player *player = ClosestPlayer();
    unsigned int frame = mFrame;
    int reach = frame * 0x22;
    if (player != 0 && frame < 0x46 && player->mIsAirborne == 0) {
        int dist = Vec3_HorzDist((const Vector3 *)&mPosX,
                                  (const Vector3 *)&player->mPosX);
        int band0 = reach * data_ov060_0211ab20[0];
        int band1 = reach * data_ov060_0211ab20[1];
        int band2 = reach * data_ov060_0211ab20[2];
        int band3 = reach * data_ov060_0211ab20[3];
        if ((band0 < dist && dist < band1) || (band2 < dist && dist < band3))
            player->Shock(1);
    }
    mTextureSequence1.Advance();
    mTextureSequence2.Advance();
    mMaterialChanger1.Advance();
    mMaterialChanger2.Advance();
    mTextureTransformer1.Advance();
    mTextureTransformer2.Advance();
    static_cast<Animation &>(mModelAnim1).Advance();
    static_cast<Animation &>(mModelAnim2).Advance();
    Matrix4x3_FromTranslation(&mModelAnim1.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    Matrix4x3_FromTranslation(&mModelAnim2.mat4x3, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    if (static_cast<Animation &>(mModelAnim1).Finished())
        MarkForDestruction();
    return 1;
}

// @symbol _ZN9daFRing_c13InitResourcesEv
s32 daFRing_c::InitResources()
{
    mModelAnim1.SetFile((BMD_File *)Model::LoadFile(*(SharedFilePtr *)&data_ov060_0211b208), 1, 0x13);
    mModelAnim2.SetFile((BMD_File *)data_ov060_0211b208[1], 1, 0x13);

    Animation::LoadFile(*(SharedFilePtr *)&data_ov060_0211b1f8);
    TextureSequence::LoadFile(*(SharedFilePtr *)&data_ov060_0211b200);

    func_02016b24(&mModelAnim1, 0x4000);
    func_02016b24(&mModelAnim2, 0x4000);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim1, (BCA_File *)data_ov060_0211b1f8[1], 0x40000000, 0x1000, 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim2, (BCA_File *)data_ov060_0211b1f8[1], 0x40000000, 0x1000, 0);

    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BTP_File *)data_ov060_0211b200[1]);
    TextureSequence::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BTP_File *)data_ov060_0211b200[1]);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence1, (BTP_File *)data_ov060_0211b200[1], 0x40000000, 0x1000, 0);
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(&mTextureSequence2, (BTP_File *)data_ov060_0211b200[1], 0x40000000, 0x1000, 0);

    MaterialChanger::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BMA_File *)func_021115e4);
    MaterialChanger::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BMA_File *)func_021115e4);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(&mMaterialChanger1, (BMA_File *)func_021115e4, 0x40000000, 0x1000, 0);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(&mMaterialChanger2, (BMA_File *)func_021115e4, 0x40000000, 0x1000, 0);

    TextureTransformer::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BTA_File *)func_021115f4);
    TextureTransformer::Prepare(*(BMD_File *)data_ov060_0211b208[1], *(BTA_File *)func_021115f4);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer1, (BTA_File *)func_021115f4, 0x40000000, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(&mTextureTransformer2, (BTA_File *)func_021115f4, 0x40000000, 0x1000, 0);

    mFrame = 0;
    return 1;
}
