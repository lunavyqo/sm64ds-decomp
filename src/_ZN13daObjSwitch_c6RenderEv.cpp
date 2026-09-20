//cpp
// @symbol _ZN13daObjSwitch_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daObjSwitch_c.h"
struct Base { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void M(void*); };

int daObjSwitch_c::Render()
{
  ((Base *)&(*(u8 *)&mModel))->M((char *)&mDrawScaleX);
  return 1;
}
