//cpp
/**
 * Whomp's Fortress sliding bars (`dossunbar`). One class, two
 * spawn profiles (BK_DOSSUNBAR_L / BK_DOSSUNBAR_S). A seven-state
 * cycle slides the mesh out from home and back.
 *
 * daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L / _S are reconstructed
 * (RTTI daObjBk_Dossunbar_c, those registry ids). Retail does not
 * store those spellings.
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile, IsClsnInRange, Particle::System::NewSimple
 *   stay mangled (Fix12-by-value, wall 6az). SetFile as a method
 *   size-DIFF Init.
 * Leftover: unk_0a4 is dActor_c's X speed (velocity at 0xa4/a8/ac),
 *   not a field of this class.
 * Leftover: resource table is two { model, KCL, CLPS } rows. Init must
 *   keep the three column symbols 02114534/38/3c; a typed
 *   ResourceDescriptor[2] size-DIFF Init and changes reloc destinations.
 *   sinit file IDs 0x58b / 0x58c / 0x58d / 0x58e.
 *   g_profile_BK_DOSSUNBAR_L / _S are not defined here (S14).
 * Leftover: func_01ffb0a4 / func_01ffb07c are MeshCollider ITCM
 *   (flag 0x35 / vec at +0x38). func_020393d4 stores the BeforeClsn
 *   callback; 020396d0 and Math_Function_0203b0fc / 0203b14c names
 *   belong with those callees.
 */

#include "daObjBk_Dossunbar_c.h"
#include "Sound.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};
typedef char ResourceDescriptor_size_must_be_0x0c[
    sizeof(ResourceDescriptor) == 0x0c ? 1 : -1];

typedef void (daObjBk_Dossunbar_c::*PMF)();
struct StateEntry { PMF handler; };

/* The installer walks the enter-table records by hand as {ptr, adj}. */
typedef void (*FnPtr)(void *);
struct VtEntry {
    int field0;
    int field1;
};

enum { kMovingBarBigActorId = 0x35 };

/* SharedFilePtr has no fields or ctor of its own, so each file handle is a
 * declaration-only subclass carrying the observed {fileId, file} layout;
 * the manifest aliases the mangled ctor/dtor spellings to the arm9
 * destinations (func_02017acc/func_02017ab4 for models,
 * func_02017b4c/SharedFilePtr_Destruct_Clsn for collision). */
struct DossunbarModelFilePtr : SharedFilePtr {
    s32 fileId;
    void *file;
    DossunbarModelFilePtr(u32 fileID);
    ~DossunbarModelFilePtr();
};

struct DossunbarCollisionFilePtr : SharedFilePtr {
    s32 fileId;
    void *file;
    DossunbarCollisionFilePtr(u32 fileID);
    ~DossunbarCollisionFilePtr();
};

extern "C" {
/* Three column symbols of one two-row {model, KCL, CLPS} table.
   A typed ResourceDescriptor[2] changes Init reloc destinations. */
extern SharedFilePtr *data_ov015_02114534;
extern SharedFilePtr *data_ov015_02114538;
extern CLPS_Block *data_ov015_0211453c;
extern StateEntry data_ov015_021149ec[];
extern PMF data_ov015_02114a24[];

void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, int, void *, int, int, void *);

void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
void func_01ffb0a4(dBgW_Kc *self);
void func_01ffb07c(dBgW_Kc *self, const Vector3 *v);
void func_020393d4(dBgW *self, void *fn);
void func_020396d0(dBgW *self, int v);
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_S()
{
    return new daObjBk_Dossunbar_c();
}

// @symbol daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L
extern "C" daObjBk_Dossunbar_c *daObjBk_Dossunbar_c_classInit_BK_DOSSUNBAR_L()
{
    return new daObjBk_Dossunbar_c();
}

// @symbol _ZN19daObjBk_Dossunbar_c13InitResourcesEv
s32 daObjBk_Dossunbar_c::InitResources()
{
    /* Temporary is load-bearing: ROM materialises 0/1 then stores. */
    int isBig = (actorID == kMovingBarBigActorId) ? 1 : 0;
    if (isBig)
        mVariant = 1;
    else
        mVariant = 0;

    int j0 = mVariant * 0xc;
    void *modelFile = Model::LoadFile(
        **(SharedFilePtr **)((char *)&data_ov015_02114534 + j0));
    mModel.SetFile((BMD_File *)modelFile, 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();

    int j = mVariant * 0xc;
    void *kclFile = dBgW_Kc::LoadFile(
        **(SharedFilePtr **)((char *)&data_ov015_02114538 + j));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider, (int)kclFile, &mClsnMat, 0x1000, mAngleY,
        *(void **)((char *)&data_ov015_0211453c + j));
    func_020393d4(
        &mMeshCollider,
        (void *)&dBgW::UpdatePosWithVelocity);

    int tmp[3];
    tmp[0] = 0x1000;
    tmp[1] = 0;
    tmp[2] = 0;
    func_01ffb0a4(&mMeshCollider);
    func_01ffb07c(&mMeshCollider, (const Vector3 *)tmp);
    func_020396d0(&mMeshCollider, 0xccd);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    this->func_ov015_02111fb8(5);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c8BehaviorEv
s32 daObjBk_Dossunbar_c::Behavior()
{
    (this->*(data_ov015_021149ec[mState].handler))();
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c6RenderEv
s32 daObjBk_Dossunbar_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c16CleanupResourcesEv
s32 daObjBk_Dossunbar_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    (*(SharedFilePtr **)((char *)&data_ov015_02114534 + mVariant * 0xc))->Release();
    (*(SharedFilePtr **)((char *)&data_ov015_02114538 + mVariant * 0xc))->Release();
    return 1;
}

/* Install mState and run that state's enter function. The enter-table
   records are raw PMF words walked by hand. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111fb8Ei
void daObjBk_Dossunbar_c::func_ov015_02111fb8(int idx)
{
    VtEntry *e = (VtEntry *)((char *)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void *obj = (void *)((char *)this + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1)
        fn = (FnPtr)*(int *)((char *)*(int **)obj + e->field0);
    else
        fn = (FnPtr)e->field0;
    fn(obj);
    this->mState = idx;
}

/* State 0 enter: wait at the out position. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111facEv
void daObjBk_Dossunbar_c::func_ov015_02111fac()
{
    this->mStateTimer = 20;
}

/* State 0 body. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111f6cEv
void daObjBk_Dossunbar_c::func_ov015_02111f6c()
{
    this->mStateTimer -= 1;
    if (this->mStateTimer > 0)
        return;
    this->func_ov015_02111fb8(1);
}

/* State 1 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111f4cEv
void daObjBk_Dossunbar_c::func_ov015_02111f4c()
{
    this->mStateTimer = 10;
    Sound::PlayBank3(0xc3, *(Vector3 *)&this->mCamSpacePosX);
}

/* State 1 body: ease X towards home minus 0x168000, then wait-mid. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111eecEv
void daObjBk_Dossunbar_c::func_ov015_02111eec()
{
    Math_Function_0203b0fc(
        &this->mPosX, this->mHomePosX - 0x168000, 0x800, 0x46000);
    this->mStateTimer -= 1;
    if (this->mStateTimer > 0)
        return;
    this->mPosX = this->mHomePosX - 0x168000;
    this->func_ov015_02111fb8(2);
}

/* State 2 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111ee0Ev
void daObjBk_Dossunbar_c::func_ov015_02111ee0()
{
    this->mStateTimer = 5;
}

/* State 2 body: small bar -> return-ease, large bar -> return-coast. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111e80Ev
void daObjBk_Dossunbar_c::func_ov015_02111e80()
{
    this->mStateTimer -= 1;
    if (this->mStateTimer > 0)
        return;
    if (this->mVariant == 0)
        this->func_ov015_02111fb8(3);
    else
        this->func_ov015_02111fb8(4);
}

/* State 3 enter. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111e60Ev
void daObjBk_Dossunbar_c::func_ov015_02111e60()
{
    this->mStateTimer = 0x18;
    Sound::PlayBank3(0xc3, *(Vector3 *)&this->mCamSpacePosX);
}

/* State 3 body: ease X back to home, then wait-home. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111df4Ev
void daObjBk_Dossunbar_c::func_ov015_02111df4()
{
    Math_Function_0203b14c(
        &this->mPosX, this->mHomePosX, 0x800, 0xb4000, 0x28000);
    this->mStateTimer -= 1;
    if (this->mStateTimer > 0)
        return;
    this->mPosX = this->mHomePosX;
    this->func_ov015_02111fb8(5);
}

/* State 4 enter: X speed rather than a timer. unk_0a4 is dActor_c's
   velocity X (with mVertSpeed / unk_0ac). */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111dd4Ev
void daObjBk_Dossunbar_c::func_ov015_02111dd4()
{
    this->unk_0a4 = 0xf000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&this->mCamSpacePosX);
}

/* State 4 body: coast until X reaches home, then wait-home. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111d98Ev
void daObjBk_Dossunbar_c::func_ov015_02111d98()
{
    this->UpdatePosWithOnlySpeed(0);
    if (this->mPosX < this->mHomePosX)
        return;
    this->mPosX = this->mHomePosX;
    this->func_ov015_02111fb8(5);
}

/* State 5 enter -- InitResources starts the cycle here. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111d8cEv
void daObjBk_Dossunbar_c::func_ov015_02111d8c()
{
    this->mStateTimer = 10;
}

/* State 5 body. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111d4cEv
void daObjBk_Dossunbar_c::func_ov015_02111d4c()
{
    this->mStateTimer -= 1;
    if (this->mStateTimer > 0)
        return;
    this->func_ov015_02111fb8(6);
}

/* State 6 enter: X speed toward the out position. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111d28Ev
void daObjBk_Dossunbar_c::func_ov015_02111d28()
{
    this->unk_0a4 = -0x14000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&this->mCamSpacePosX);
}

/* State 6 body: travel until X is 0x1ea000 below home, then wait-out. */
// @symbol _ZN19daObjBk_Dossunbar_c19func_ov015_02111ce0Ev
void daObjBk_Dossunbar_c::func_ov015_02111ce0()
{
    this->UpdatePosWithOnlySpeed(0);
    int v = this->mHomePosX + (int)0xffe16000;
    if (this->mPosX > v)
        return;
    this->mPosX = v;
    this->func_ov015_02111fb8(0);
}

// @symbol _ZN19daObjBk_Dossunbar_c15OnHitByMegaCharER6Player
void daObjBk_Dossunbar_c::OnHitByMegaChar(Player &player)
{
    player.IncMegaKillCount();
    Kill();
}

// @symbol _ZN19daObjBk_Dossunbar_c4KillEv
void daObjBk_Dossunbar_c::Kill()
{
    Vector3 pos;
    Vector3 dustPos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0xc8000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x10a, pos.x, pos.y, pos.z);
    dustPos.x = pos.x;
    dustPos.y = pos.y;
    dustPos.z = pos.z;
    PoofDustAt(dustPos);
    Sound::PlayBank3(0x41, *(Vector3 *)&mCamSpacePosX);
    MarkForDestruction();
}

/* Construction order is source order: the four file handles first, then
 * the enter-table and the handler-table rows in the order the initializer
 * copies them. The registration nodes and the PMF literals are compiler
 * temporaries. */
DossunbarModelFilePtr data_ov015_021149a4(0x58d);
DossunbarCollisionFilePtr data_ov015_021149ac(0x58e);
DossunbarModelFilePtr data_ov015_021149b4(0x58b);
DossunbarCollisionFilePtr data_ov015_0211499c(0x58c);

PMF data_ov015_02114a24[7] = {
    &daObjBk_Dossunbar_c::func_ov015_02111fac,
    &daObjBk_Dossunbar_c::func_ov015_02111f4c,
    &daObjBk_Dossunbar_c::func_ov015_02111ee0,
    &daObjBk_Dossunbar_c::func_ov015_02111e60,
    &daObjBk_Dossunbar_c::func_ov015_02111dd4,
    &daObjBk_Dossunbar_c::func_ov015_02111d8c,
    &daObjBk_Dossunbar_c::func_ov015_02111d28,
};
StateEntry data_ov015_021149ec[7] = {
    {&daObjBk_Dossunbar_c::func_ov015_02111f6c},
    {&daObjBk_Dossunbar_c::func_ov015_02111eec},
    {&daObjBk_Dossunbar_c::func_ov015_02111e80},
    {&daObjBk_Dossunbar_c::func_ov015_02111df4},
    {&daObjBk_Dossunbar_c::func_ov015_02111d98},
    {&daObjBk_Dossunbar_c::func_ov015_02111d4c},
    {&daObjBk_Dossunbar_c::func_ov015_02111ce0},
};
