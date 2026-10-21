#include "types.h"
extern u32 _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(u32 tok);
extern volatile u32 data_020a6438[2];

long long func_02059650(void)
{
    u32 tok = _ZN3IRQ7DisableEv();
    volatile u16 timer = *(volatile u16*)0x4000100;
    volatile unsigned long long counter = *(volatile unsigned long long*)data_020a6438 & 0xffffffffffffULL;
    if ((*(volatile u32*)0x4000214 & 8) && !(timer & 0x8000))
        counter = counter + 1;
    _ZN3IRQ7RestoreEj(tok);
    return ((long long)counter << 16) | (int)(timer);
}

typedef unsigned long long u64;
extern volatile u64 data_020a6438;
extern int data_020a6434;
extern void func_02056e4c(int a, void *b, int c);
extern void func_02059700(void);

void func_02059700(void) {
    data_020a6438++;
    if (data_020a6434 != 0) {
        *(volatile unsigned short *)0x4000102 = 0;
        *(volatile unsigned short *)0x4000100 = 0;
        *(volatile unsigned short *)0x4000102 = 0xc1;
        data_020a6434 = 0;
    }
    func_02056e4c(0, (void *)func_02059700, 0);
}

extern unsigned short data_020a6430;
extern int data_020a6438;
extern int data_020a6434;
extern void func_02059700(void);
extern void func_02059624(int bit);
extern void _ZN3IRQ13SetIRQHandlerEjPFvvE(unsigned int bit, void (*handler)(void));
extern void _ZN3IRQ10EnableIRQsEj(unsigned int bits);

void func_02059788(void) {
    if (data_020a6430 != 0)
        return;
    data_020a6430 = 1;
    func_02059624(0);
    (&data_020a6438)[0] = 0;
    (&data_020a6438)[1] = 0;
    *(volatile unsigned short *)0x4000102 = 0;
    *(volatile unsigned short *)0x4000100 = 0;
    *(volatile unsigned short *)0x4000102 = 0xc1;
    _ZN3IRQ13SetIRQHandlerEjPFvvE(8, func_02059700);
    _ZN3IRQ10EnableIRQsEj(8);
    data_020a6434 = 0;
}
