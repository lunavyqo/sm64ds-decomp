//cpp
// @symbol _ZN7daRNk_c6RenderEv
/* recovered: named members + shared header, real C++ method */
#include "daRNk_c.h"
/* Model is the real class now, through daRNk_c.h: HideMaterial is its own
   non-virtual and the slot-5 virtual is Render, which ModelAnim overrides. */

int daRNk_c::Render()
{
  mModelAnim.HideMaterial(0, 1);
  mModelAnim.Render((const Vector3*)((char*)&mScaleX));
  return 1;
}
