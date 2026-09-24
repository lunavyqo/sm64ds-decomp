//cpp
/* ov026 daWater_Suikomi_c: the five _ZN17daWater_Suikomi_c* methods that form one
 * gap-free complete run, 0x02112328..0x02112450. D1/D0 sit before
 * daWater_Tatumaki_c's extra slots, and daWater_Suikomi_c_classInit is the
 * next function. Neither is part of this run.
 *
 * #pragma defer_codegen off emits in source order, which is ROM order.
 */
#include "decl_common.h"
#include "daWater_Suikomi_c.h"

struct Obj;
struct C {
    char pad[0x30c];
    Obj *obj;
};
typedef void (C::*PMF)();
struct Obj {
    char pad[8];
    PMF pmf;
};

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
struct V3 {
    int x, y, z;
};
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, V3 *vec, int a, int b, unsigned int cc, unsigned int d);
extern V3 data_ov026_02113f4c;
}

#pragma defer_codegen off

// @symbol _ZN17daWater_Suikomi_c16CleanupResourcesEv
int daWater_Suikomi_c::CleanupResources()
{
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c16OnPendingDestroyEv
void daWater_Suikomi_c::OnPendingDestroy()
{
}

// @symbol _ZN17daWater_Suikomi_c6RenderEv
int daWater_Suikomi_c::Render()
{
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c8BehaviorEv
int daWater_Suikomi_c::Behavior()
{
    C *c = (C *)this;
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    Obj *o = c->obj;
    if (*(int *)((char *)o + 8) != 0) {
        (c->*(o->pmf))();
    }
    UpdatePos(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov026_02112324(this);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c13InitResourcesEv
int daWater_Suikomi_c::InitResources()
{
    V3 vec;
    unk_314 = param1 & 0xff;
    vec = data_ov026_02113f4c;
    /* Leftover: _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj.
       dCcAcPos_c::Init (Fix12<int> by value) DIFF'd InitResources. */
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &vec, 0x64000, 0xa4000, 0x800006, 0);
    func_ov026_021122d4(this, &data_ov026_02113f58);
    return 1;
}
