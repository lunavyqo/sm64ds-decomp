//cpp
// @symbol _ZN6PlayerC1Ev
/* recovered: named members + shared header */
#include "Player.h"
extern "C" {
extern int data_ov002_0210a83c[];
void *_ZN8dActor_cC2Ev(void*);
void *_ZN9ModelAnimC1Ev(void*);
void __cxa_vec_ctor(void*,int,int,void*,void*);
void *_ZN15TextureSequenceC1Ev(void*);
void _ZN15TextureSequenceD1Ev(void*);
void *_ZN15MaterialChangerC1Ev(void*);
void _ZN15MaterialChangerD1Ev(void*);
void *_ZN17dExtShadowModel_cC1Ev(void*);
void *_ZN10dCcAcPos_cC1Ev(void*);
void *_ZN10dBgCh_ActrC1Ev(void*);
void* _ZN6PlayerC1Ev(void* c){
  char* p=(char*)c;
  _ZN8dActor_cC2Ev(c);
  char* r4=p+0xdc;
  *(int*)p=(int)data_ov002_0210a83c;
  _ZN9ModelAnimC1Ev(r4+0x14);
  _ZN9ModelAnimC1Ev(r4+0x98);
  __cxa_vec_ctor(r4+0x100,4,0x14,(void*)_ZN15TextureSequenceC1Ev,(void*)_ZN15TextureSequenceD1Ev);
  __cxa_vec_ctor(r4+0x150,2,0x14,(void*)_ZN15MaterialChangerC1Ev,(void*)_ZN15MaterialChangerD1Ev);
  __cxa_vec_ctor(r4+0x178,2,0x14,(void*)_ZN15TextureSequenceC1Ev,(void*)_ZN15TextureSequenceD1Ev);
  _ZN17dExtShadowModel_cC1Ev(p+0x2ac);
  _ZN10dCcAcPos_cC1Ev(p+0x2d4);
  _ZN10dCcAcPos_cC1Ev(p+0x314);
  _ZN10dBgCh_ActrC1Ev(p+0x380);
  return c;
}
}

#pragma define_section forcedata ".data" ".data" ".data" data_mode=far_abs
#pragma section forcedata begin
extern "C" int data_ov002_0210a83c[31] = { 0x020e558c, 0x02011268, 0x02011244, 0x020e32d4, 0x02011220, 0x02011214, 0x020e4d24, 0x02010fd4, 0x02010fc8, 0x020e3a08, 0x02010f78, 0x02010f6c, 0x020e3a04, 0x0204357c, 0x0204349c, 0x02043494, 0x020e6858, 0x020e67a8, 0x020e69b8, 0x02010154, 0x0201014c, 0x02010148, 0x02010144, 0x02010140, 0x0201013c, 0x02010138, 0x02010134, 0x02010130, 0x0201012c, 0x02010124, 0x020100dc };
#pragma section forcedata end
