//cpp
/* G2 / G2S -- BG character and screen base pointers for the main (G2,
 * 0x04000000) and sub (G2S, 0x04001000) 2D engines, read from the display
 * control registers (arm9 .text 0x02054d38..0x020551f0).
 *
 * The two namespaces interleave one by one in ROM, so one TU holds both
 * (same rule as the GX/GXS family). G2 and G2S are namespaces, not
 * classes: no `this`, no vtable, no layout. mwccarm emits .text in
 * reverse source order, so the definitions run ROM-descending below.
 * The three address-named helpers keep C linkage under their names.
 *
 */
// @symbol _ZN2G212GetBG0ScrPtrEv
namespace G2 {
void* GetBG0ScrPtr(void)
{
  int off = ((*(volatile unsigned short*)0x4000008) & 0x1f00) >> 8;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x38000000;
  return (void*)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}
}

// @symbol _ZN3G2S12GetBG0ScrPtrEv
namespace G2S {
unsigned int GetBG0ScrPtr()
{
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}
}

// @symbol _ZN2G212GetBG1ScrPtrEv
namespace G2 {
void*  GetBG1ScrPtr(void)
{
  int off = ((*(volatile unsigned short*)0x400000a) & 0x1f00) >> 8;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x38000000;
  return (void*)(0x6000000 + ((sbase >> 0x1b) << 0x10) + (off << 11));
}
}

// @symbol _ZN3G2S12GetBG1ScrPtrEv
namespace G2S {
unsigned int GetBG1ScrPtr()
{
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x1f00) >> 8) << 0xb) + 0x6200000;
}
}

// @symbol _ZN2G212GetBG2ScrPtrEv
namespace G2 {
void*  GetBG2ScrPtr(void)
{
  unsigned int mode = *(volatile unsigned int*)0x4000000 & 7;
  unsigned short bg2cnt = *(volatile unsigned short*)0x400000c;
  unsigned int sbase = ((*(volatile unsigned int*)0x4000000 & 0x38000000) >> 0x1b) << 0x10;
  unsigned int off = ((unsigned int)(bg2cnt & 0x1f00)) >> 8;
  switch(mode){
  case 0: case 1: case 2: case 3: case 4:
    return (void*)(0x6000000 + sbase + (off << 11));
  case 5:
    if(bg2cnt & 0x80) return (void*)(0x6000000 + (off << 0xe));
    return (void*)(0x6000000 + sbase + (off << 11));
  case 6:
    return (void*)0x6000000;
  default:
    return (void*)0;
  }
}
}

// @symbol _ZN3G2S12GetBG2ScrPtrEv
namespace G2S {
unsigned int GetBG2ScrPtr()
{
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100c;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        return (r1 << 0xb) + 0x6200000;
    case 5:
        if (v & 0x80) return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}
}

// @symbol _ZN2G212GetBG3ScrPtrEv
namespace G2 {
void*  GetBG3ScrPtr()
{
  unsigned int mode = *(volatile unsigned int*)0x4000000 & 7;
  unsigned short bg3cnt = *(volatile unsigned short*)0x400000e;
  unsigned int sbase = ((*(volatile unsigned int*)0x4000000 & 0x38000000) >> 0x1b) << 0x10;
  unsigned int off = ((unsigned int)(bg3cnt & 0x1f00)) >> 8;
  switch(mode){
  case 0: case 1: case 2:
    return (void*)(0x6000000 + sbase + (off << 11));
  case 3: case 4: case 5:
    if(bg3cnt & 0x80) return (void*)(0x6000000 + (off << 0xe));
    return (void*)(0x6000000 + sbase + (off << 11));
  case 6:
    return (void*)0;
  default:
    return (void*)0;
  }
}
}

// @symbol _ZN3G2S12GetBG3ScrPtrEv
namespace G2S {
unsigned int GetBG3ScrPtr()
{
    int m = *(volatile int *)0x4001000 & 7;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    unsigned int r1 = (v & 0x1f00) >> 8;
    switch (m) {
    case 0:
    case 1:
    case 2:
        return (r1 << 0xb) + 0x6200000;
    case 3:
    case 4:
    case 5:
        if (v & 0x80) return (r1 << 0xe) + 0x6200000;
        return (r1 << 0xb) + 0x6200000;
    case 6:
        return 0;
    default:
        return 0;
    }
}
}

// @symbol func_02054efc
extern "C" {
void* func_02054efc(void){
  int off = ((*(volatile unsigned short*)0x4000008) & 0x3c) >> 2;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x7000000;
  return (void*)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}
}

// @symbol _ZN3G2S13GetBG0CharPtrEv
namespace G2S {
unsigned int GetBG0CharPtr()
{
    int v = *(volatile unsigned short *)0x4001008;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}
}

// @symbol func_02054ea8
extern "C" {
void* func_02054ea8(void){
  int off = ((*(volatile unsigned short*)0x400000a) & 0x3c) >> 2;
  unsigned int sbase = (*(volatile unsigned int*)0x4000000) & 0x7000000;
  return (void*)(0x6000000 + ((sbase >> 0x18) << 0x10) + (off << 14));
}
}

// @symbol _ZN3G2S13GetBG1CharPtrEv
namespace G2S {
unsigned int GetBG1CharPtr()
{
    int v = *(volatile unsigned short *)0x400100a;
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
}
}

// @symbol _ZN2G213GetBG2CharPtrEv
namespace G2 {
void*  GetBG2CharPtr(void)
{
  int mode = *(volatile unsigned int*)0x4000000 & 7;
  unsigned int bg2cnt = *(volatile unsigned short*)0x400000c;
  if(mode < 5 || !(bg2cnt & 0x80))
    return (void*)(0x6000000
      + (((*(volatile unsigned int*)0x4000000 & 0x7000000) >> 0x18) << 0x10)
      + (((bg2cnt & 0x3c) >> 2) << 14));
  return (void*)0;
}
}

// @symbol _ZN3G2S13GetBG2CharPtrEv
namespace G2S {
unsigned GetBG2CharPtr()
{
  int v1 = *(volatile int*)0x4001000;
  unsigned short v2 = *(volatile unsigned short*)0x400100c;
  if (!((v1 & 7) >= 5 && (v2 & 0x80)))
    return ((unsigned)(v2 & 0x3c) >> 2 << 0xe) + 0x6200000;
  return 0;
}
}

// @symbol func_02054d88
extern "C" {
int func_02054d88(void)
{
    unsigned int mode = *(volatile unsigned int *)0x4000000 & 7;
    unsigned short h = *(volatile unsigned short *)0x400000e;
    if ((int)mode < 3 || ((int)mode < 6 && !(h & 0x80))) {
        unsigned int v = *(volatile unsigned int *)0x4000000;
        return 0x6000000 + (((v & 0x7000000) >> 0x18) << 0x10) + (((unsigned int)(h & 0x3c) >> 2) << 14);
    }
    return 0;
}
}

// @symbol _ZN3G2S13GetBG3CharPtrEv
namespace G2S {
unsigned int GetBG3CharPtr()
{
    int cfg = *(volatile int *)0x4001000;
    unsigned int v = *(volatile unsigned short *)0x400100e;
    int m = cfg & 7;
    if (m >= 3) {
        if (m >= 6) goto zero;
        if (v & 0x80) goto zero;
    }
    return (((v & 0x3c) >> 2) << 0xe) + 0x6200000;
zero:
    return 0;
}
}
