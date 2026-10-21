extern void func_02057410(int a, unsigned int b, int c, unsigned int d);

void func_02057ce8(int a, int b, int c)
{
    unsigned int m;
    m = 0;
    m = ~m;
    m >>= 1;
    func_02057410(a, m, b, (unsigned int)c);
}


#include "types.h"
extern void func_02057ce8(void *arg0, u32 fmtAddr, u32 val);

void func_02057d00(void *arg0, u32 fmt, ...)
{
    u32 ap = (((u32)&fmt) & ~3u) + 4;
    func_02057ce8(arg0, fmt, ap);
}
