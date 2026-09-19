//cpp
// @symbol _ZN9daPopoi_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daPopoi_c.h"
extern int data_0209f32c;

struct Cls {
    virtual void method0();
    virtual void method1();
    virtual void method2();
    virtual void method3();
    virtual void method4();
    virtual void method5(int);  /* at vtable offset 0x14 */
};

int daPopoi_c::Render()
{
    if (mPosY < data_0209f32c) return 1;
    Cls *obj = (Cls*)((char*)&mModelAnim);
    obj->method5(0);
    return 1;
}
