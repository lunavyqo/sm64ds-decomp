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
 * Leftover: state bodies stay extern "C" (Dossunbar_*). A C++ method
 *   would emit _ZN19daObjBk_Dossunbar_c* and miss the unowned PMF
 *   records. Klass stays incomplete: completing it as this class
 *   changes PMF representation. Tables 02114a24 / 021149ec are
 *   filled by __sinit_ov015_02113048, not this TU.
 * Leftover: Model::LoadFile, ModelBase::SetFile, dBgW_Kc::LoadFile,
 *   dBgW_KcMbg::SetFile, IsClsnInRange, Particle::System::NewSimple
 *   stay mangled (Fix12-by-value, wall 6az).
 * Leftover: two spellings of +0x334 (decrement vs re-read). A second
 *   mStateTimer load CSEs. unk_0a4 is dActor_c's X speed (velocity
 *   at 0xa4/a8/ac), not a field of this class.
 * Leftover: resource table is two { model, KCL, CLPS } rows. Init and
 *   Cleanup must keep the three column symbols 02114534/38/3c; a typed
 *   ResourceDescriptor row changes reloc destinations. sinit file IDs
 *   0x58b / 0x58c / 0x58d / 0x58e. Text-only TU, so
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

struct Klass;
typedef void (Klass::*PMF)();
struct StateEntry { PMF handler; };

/* The installer walks the enter-table records by hand as {ptr, adj}. */
typedef void (*FnPtr)(void *);
struct VtEntry {
    int field0;
    int field1;
};

enum { kMovingBarBigActorId = 0x35 };

extern "C" {
/* Three column symbols of one two-row {model, KCL, CLPS} table.
   A typed ResourceDescriptor[2] changes Init/Cleanup reloc destinations. */
extern char data_ov015_02114534[];
extern char data_ov015_02114538[];
extern int data_ov015_0211453c[];
extern StateEntry data_ov015_021149ec[];
extern VtEntry data_ov015_02114a24[];

void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, Fix12i x, Fix12i y, Fix12i z);
void _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(void *, void *);
void _ZN10dBgActor_c21UpdateModelPosAndRotYEv(void *);
void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *);
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *, int, int);
int _ZN5Model8LoadFileER13SharedFilePtr(void *);
int _ZN9ModelBase7SetFileEP8BMD_Fileii(void *, int, int, int);
int _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(void *);
int _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    void *, int, void *, int, int, void *);
int _ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_;

void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void Math_Function_0203b14c(int *p, int target, int scale, int max, int extra);
int func_01ffb0a4(void *);
int func_01ffb07c(void *, void *);
void func_020393d4(int *p, int v);
void func_020396d0(int *p, int v);

void Dossunbar_SetState(daObjBk_Dossunbar_c *self, int idx);
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
    int modelFile = _ZN5Model8LoadFileER13SharedFilePtr(
        *(void **)((char *)data_ov015_02114534 + j0));
    _ZN9ModelBase7SetFileEP8BMD_Fileii((char *)&mModel, modelFile, 1, -1);
    _ZN10dBgActor_c21UpdateModelPosAndRotYEv((char *)this);
    _ZN10dBgActor_c19UpdateClsnPosAndRotEv((char *)this);

    int j = mVariant * 0xc;
    int kclFile = _ZN7dBgW_Kc8LoadFileER13SharedFilePtr(
        *(void **)((char *)data_ov015_02114538 + j));
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        (char *)&mMeshCollider, kclFile, (char *)&mClsnMat, 0x1000, mAngleY,
        *(void **)((char *)data_ov015_0211453c + j));
    func_020393d4(
        (int *)&mMeshCollider,
        (int)&_ZN4dBgW21UpdatePosWithVelocityERS_P8dActor_cR5dBgPiR7Vector3P10Vector3_16S8_);

    int tmp[3];
    tmp[0] = 0x1000;
    tmp[1] = 0;
    tmp[2] = 0;
    func_01ffb0a4((char *)&mMeshCollider);
    func_01ffb07c((char *)&mMeshCollider, tmp);
    func_020396d0((int *)&mMeshCollider, 0xccd);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    Dossunbar_SetState(this, 5);
    return 1;
}

// @symbol _ZN19daObjBk_Dossunbar_c8BehaviorEv
s32 daObjBk_Dossunbar_c::Behavior()
{
    (((Klass *)this)->*(data_ov015_021149ec[mState].handler))();
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
    /* Must call through dBgW*, not dBgW_KcMbg. The u8 overlay is how
       the ROM takes that address; a local dBgW* size-DIFF. */
    if (((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->IsEnabled())
        ((dBgW *)((char *)&(*(u8 *)&mMeshCollider)))->Disable();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114534 + mVariant * 0xc)))->Release();
    ((SharedFilePtr *)(*(void **)(data_ov015_02114538 + mVariant * 0xc)))->Release();
    return 1;
}

/* Install mState and run that state's enter function. */
// @symbol Dossunbar_SetState
extern "C" void Dossunbar_SetState(daObjBk_Dossunbar_c *self, int idx)
{
    VtEntry *e = (VtEntry *)((char *)data_ov015_02114a24 + (idx << 3));
    int f1 = e->field1;
    void *obj = (void *)((char *)self + (f1 >> 1));
    FnPtr fn;
    if (f1 & 1)
        fn = (FnPtr)*(int *)((char *)*(int **)obj + e->field0);
    else
        fn = (FnPtr)e->field0;
    fn(obj);
    self->mState = idx;
}

/* State 0 enter: wait at the out position. */
// @symbol Dossunbar_WaitOutEnter
extern "C" void Dossunbar_WaitOutEnter(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 20;
}

/* State 0 body. */
// @symbol Dossunbar_WaitOut
extern "C" void Dossunbar_WaitOut(daObjBk_Dossunbar_c *self)
{
    *(int *)(((int)self + 0x334)) -= 1;
    if (*(int *)((char *)self + 0x334) > 0)
        return;
    Dossunbar_SetState(self, 1);
}

/* State 1 enter. */
// @symbol Dossunbar_SlideInEnter
extern "C" void Dossunbar_SlideInEnter(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 10;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 1 body: ease X towards home minus 0x168000, then wait-mid. */
// @symbol Dossunbar_SlideIn
extern "C" void Dossunbar_SlideIn(daObjBk_Dossunbar_c *self)
{
    Math_Function_0203b0fc(
        &self->mPosX, self->mHomePosX - 0x168000, 0x800, 0x46000);
    {
        int *p = (int *)(((int)self + 0x334));
        *p = *p - 1;
    }
    if (*(int *)((char *)self + 0x334) > 0)
        return;
    self->mPosX = self->mHomePosX - 0x168000;
    Dossunbar_SetState(self, 2);
}

/* State 2 enter. */
// @symbol Dossunbar_WaitMidEnter
extern "C" void Dossunbar_WaitMidEnter(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 5;
}

/* State 2 body: small bar -> return-ease, large bar -> return-coast. */
// @symbol Dossunbar_WaitMid
extern "C" void Dossunbar_WaitMid(daObjBk_Dossunbar_c *self)
{
    /* Two spellings of +0x334 are load-bearing: the decrement keeps
       `add r2,r0,#0x334` and the re-read is `ldr r1,[r0,#0x334]`. */
    *(int *)((int)self + 0x334) -= 1;
    if (*(int *)((char *)self + 0x334) > 0)
        return;
    if (self->mVariant == 0)
        Dossunbar_SetState(self, 3);
    else
        Dossunbar_SetState(self, 4);
}

/* State 3 enter. */
// @symbol Dossunbar_ReturnEaseEnter
extern "C" void Dossunbar_ReturnEaseEnter(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 0x18;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 3 body: ease X back to home, then wait-home. */
// @symbol Dossunbar_ReturnEase
extern "C" void Dossunbar_ReturnEase(daObjBk_Dossunbar_c *self)
{
    Math_Function_0203b14c(
        &self->mPosX, self->mHomePosX, 0x800, 0xb4000, 0x28000);
    *(int *)(((int)self + 0x334)) =
        *(int *)(((int)self + 0x334)) - 1;
    if (*(int *)((char *)self + 0x334) > 0)
        return;
    self->mPosX = self->mHomePosX;
    Dossunbar_SetState(self, 5);
}

/* State 4 enter: X speed rather than a timer. unk_0a4 is dActor_c's
   velocity X (with mVertSpeed / unk_0ac). */
// @symbol Dossunbar_ReturnCoastEnter
extern "C" void Dossunbar_ReturnCoastEnter(daObjBk_Dossunbar_c *self)
{
    self->unk_0a4 = 0xf000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 4 body: coast until X reaches home, then wait-home. */
// @symbol Dossunbar_ReturnCoast
extern "C" void Dossunbar_ReturnCoast(daObjBk_Dossunbar_c *self)
{
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(self, 0);
    if (self->mPosX < self->mHomePosX)
        return;
    self->mPosX = self->mHomePosX;
    Dossunbar_SetState(self, 5);
}

/* State 5 enter -- InitResources starts the cycle here. */
// @symbol Dossunbar_WaitHomeEnter
extern "C" void Dossunbar_WaitHomeEnter(daObjBk_Dossunbar_c *self)
{
    self->mStateTimer = 10;
}

/* State 5 body. */
// @symbol Dossunbar_WaitHome
extern "C" void Dossunbar_WaitHome(daObjBk_Dossunbar_c *self)
{
    *(int *)(((int)self + 0x334)) -= 1;
    if (*(int *)((char *)self + 0x334) > 0)
        return;
    Dossunbar_SetState(self, 6);
}

/* State 6 enter: X speed toward the out position. */
// @symbol Dossunbar_SlideOutEnter
extern "C" void Dossunbar_SlideOutEnter(daObjBk_Dossunbar_c *self)
{
    self->unk_0a4 = -0x14000;
    Sound::PlayBank3(0xc3, *(Vector3 *)&self->mCamSpacePosX);
}

/* State 6 body: travel until X is 0x1ea000 below home, then wait-out. */
// @symbol Dossunbar_SlideOut
extern "C" void Dossunbar_SlideOut(daObjBk_Dossunbar_c *self)
{
    _ZN8dActor_c22UpdatePosWithOnlySpeedEP5dCc_c(self, 0);
    int v = self->mHomePosX + (int)0xffe16000;
    if (self->mPosX > v)
        return;
    self->mPosX = v;
    Dossunbar_SetState(self, 0);
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
