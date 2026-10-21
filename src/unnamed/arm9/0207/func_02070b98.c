#include "types.h"
void func_02070c68(void* out, double x);
void func_020715e0(void* thiz, int val);

#pragma opt_strength_reduction off
void func_02070b98(void* a0, double x, void* d)
{
    int n;
    char* c = (char*)d;
    int i;
    n = *(s16*)((char*)a0 + 2);
    func_02070c68(c, x);
    if (*(u8*)(c + 5) > 9) return;
    if (n > 0x20) n = 0x20;
    func_020715e0(c, n);
    while (*(u8*)(c + 4) < n) {
        int t = *(u8*)(c + 4);
        *(u8*)(c + 4) += 1;
        (c + t)[5] = 0;
    }
    *(s16*)(((int)c + 2)) -= *(u8*)(c + 4) - 1;
    for (i = 0; i < *(u8*)(c + 4); i++) {
        *(u8*)(((int)c + i + 5)) += 0x30;
    }
}


extern int func_01ff9d40(double, double);
extern double func_01ff8e10(double, double);
extern double func_020707f4(double, int *);
extern double func_020708b4(double, int);
extern int func_02071708(int);
extern unsigned long long func_01ff859c(double);
extern void func_02071510(void *, unsigned long long);
extern void func_02070ec0(void *, int);
extern void func_02071364(void *, void *, void *);

void func_02070c68(char *out, double x)
{
    int e;
    double t1;
    double t2;
    double mm;
    unsigned char dec[0x26];
    unsigned char big[0x26];
    double v = x;
    unsigned char neg = (((int *)&x)[1] & 0x80000000) != 0;
    int cls;
    int cls2;

    if (func_01ff9d40(0.0, v) != 0) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;
        out[5] = 0;
        return;
    }

    t1 = v;

    /* fpclassify, open-coded: an inline helper's temporaries would take
       frame slots after mm and reorder the stack. */
    switch (((int *)&t1)[1] & 0x7ff00000) {
    case 0x7ff00000:
        if ((((int *)&t1)[1] & 0xfffff) != 0 || ((int *)&t1)[0] != 0)
            cls = 1;
        else
            cls = 2;
        break;
    case 0:
        if ((((int *)&t1)[1] & 0xfffff) != 0 || ((int *)&t1)[0] != 0)
            cls = 5;
        else
            cls = 3;
        break;
    default:
        cls = 4;
        break;
    }
    if (cls <= 2) {
        out[0] = neg;
        *(short *)(out + 2) = 0;
        out[4] = 1;

        t2 = x;
        switch (((int *)&t2)[1] & 0x7ff00000) {
        case 0x7ff00000:
            if ((((int *)&t2)[1] & 0xfffff) != 0 || ((int *)&t2)[0] != 0)
                cls2 = 1;
            else
                cls2 = 2;
            break;
        case 0:
            if ((((int *)&t2)[1] & 0xfffff) != 0 || ((int *)&t2)[0] != 0)
                cls2 = 5;
            else
                cls2 = 3;
            break;
        default:
            cls2 = 4;
            break;
        }
        out[5] = (cls2 == 1) ? 0x4e : 0x49;
    } else {
        double m;
        int nbits, k;
        int *mp;

        if (neg) x = func_01ff8e10(0.0, v);
        m = func_020707f4(x, &e);
        mm = m;
        mp = (int *)&mm;
        if (mp[0] != 0)
            nbits = func_02071708(mp[0]);
        else
            nbits = func_02071708(mp[1] | 0x100000) + 0x20;
        k = 0x35 - nbits;
        func_02070ec0(big, e - k);
        func_02071510(dec, func_01ff859c(func_020708b4(m, k)));
        func_02071364(out, dec, big);
        out[0] = neg;
    }
}


typedef struct Big {
    short v[19];
} Big;

extern unsigned char data_0209a628[];
extern unsigned char data_0209a658[];
extern unsigned char data_0209a680[];
extern unsigned char data_0209a698[];
extern unsigned char data_0209a6a8[];
extern unsigned char data_0209a6b0[];
extern unsigned char data_0209a6b8[];
extern unsigned char data_0209a6c0[];
extern unsigned char data_0209a6c8[];
extern unsigned char data_0209a6cc[];
extern unsigned char data_0209a6d0[];
extern unsigned char data_0209a6d4[];
extern unsigned char data_0209a6d8[];
extern unsigned char data_0209a6dc[];
extern unsigned char data_0209a6e0[];
extern unsigned char data_0209a6e4[];
extern unsigned char data_0209a6e8[];
extern unsigned char data_0209a6ec[];
extern unsigned char data_0209a6f0[];
extern unsigned char data_0209a6f4[];
extern unsigned char data_0209a6f8[];

void func_020712a0(Big *out, unsigned char *str, short val);
void func_02071364(Big *dst, Big *a, Big *b);
void func_02070ec0(Big *out, int n);

void func_02070ec0(Big *out, int n)
{
    Big half;
    Big copy;

    switch (n) {
    case -64: func_020712a0(out, data_0209a628, -20); return;
    case -53: func_020712a0(out, data_0209a658, -16); return;
    case -32: func_020712a0(out, data_0209a680, -10); return;
    case -16: func_020712a0(out, data_0209a698, -5); return;
    case -8:  func_020712a0(out, data_0209a6a8, -3); return;
    case -7:  func_020712a0(out, data_0209a6b0, -3); return;
    case -6:  func_020712a0(out, data_0209a6b8, -2); return;
    case -5:  func_020712a0(out, data_0209a6c0, -2); return;
    case -4:  func_020712a0(out, data_0209a6c8, -2); return;
    case -3:  func_020712a0(out, data_0209a6cc, -1); return;
    case -2:  func_020712a0(out, data_0209a6d0, -1); return;
    case -1:  func_020712a0(out, data_0209a6d4, -1); return;
    case 0:   func_020712a0(out, data_0209a6d8, 0); return;
    case 1:   func_020712a0(out, data_0209a6dc, 0); return;
    case 2:   func_020712a0(out, data_0209a6e0, 0); return;
    case 3:   func_020712a0(out, data_0209a6e4, 0); return;
    case 4:   func_020712a0(out, data_0209a6e8, 1); return;
    case 5:   func_020712a0(out, data_0209a6ec, 1); return;
    case 6:   func_020712a0(out, data_0209a6f0, 1); return;
    case 7:   func_020712a0(out, data_0209a6f4, 2); return;
    case 8:   func_020712a0(out, data_0209a6f8, 2); return;
    default:
        func_02070ec0(&half, (int)(n + ((n & 0x80000000u) >> 31)) >> 1);
        func_02071364(out, &half, &half);
        if (n & 1) {
            copy = *out;
            if (n > 0)
                func_020712a0(&half, data_0209a6dc, 0);
            else
                func_020712a0(&half, data_0209a6d4, -1);
            func_02071364(out, &copy, &half);
        }
        return;
    }
}


#include "Decimal.h"

extern void func_02071644(Decimal *result, int length);

void func_020712a0(Decimal *result, unsigned char *digits, short exp)
{
    int i;
    unsigned char c;
    result->exp = exp;
    result->sign = 0;
    i = 0;
    while (i < 0x20 && digits[0] != 0) {
        result->sig.text[i++] = (unsigned char)(*digits++ - 0x30);
    }
    result->sig.length = (unsigned char)i;
    if (digits[0] == 0) return;
    if (digits[0] < 5) return;
    if (digits[0] <= 5) {
        c = digits[1];
        digits++;
        while (c != 0) {
            /* Kept goto: break plus a `c == 0` guard (+8), while (c == '0') (+4),
               for (;;) with the return inside (-4), a duplicated call (+20) and
               an inline must-round predicate (+12..+20) all change the object. */
            if (c != 0x30)
                goto docall;
            c = *++digits;
        }
        if ((result->sig.text[i - 1] & 1) == 0)
            return;
    }
docall:
    func_02071644(result, result->sig.length);
}


#include "Decimal.h"

extern void func_02071644(Decimal *result, int length);

void func_02071364(Decimal *result, const Decimal *x, const Decimal *y)
{
    unsigned int accumulator = 0;
    unsigned char mantissa[64];
    int i = x->sig.length + y->sig.length - 1;
    unsigned char *ip = mantissa + i + 1;
    unsigned char *ep = ip;

    result->sign = 0;

    for (; i > 0; i--) {
        int k = y->sig.length - 1;
        int j = i - k - 1;
        int l;
        int t;
        const unsigned char *jp;
        const unsigned char *kp;

        if (j < 0) {
            j = 0;
            k = i - 1;
        }

        jp = x->sig.text + j;
        kp = y->sig.text + k;
        l = k + 1;
        t = x->sig.length - j;

        if (l > t) {
            l = t;
        }

        for (; l > 0; l--, jp++, kp--) {
            accumulator += *jp * *kp;
        }

        *--ip = (unsigned char)(accumulator % 10);
        accumulator /= 10;
    }

    result->exp = (short)(x->exp + y->exp);

    if (accumulator) {
        short *exp = (short *)(int)((char *)result + 2);
        *--ip = (unsigned char)accumulator;
        *exp = *exp + 1;
    }

    for (i = 0; i < 32 && ip < ep; i++, ip++) {
        result->sig.text[i] = *ip;
    }
    result->sig.length = (unsigned char)i;

    if (ip < ep && *ip >= 5) {
        if (*ip == 5) {
            unsigned char *jp = ip + 1;
            for (; jp < ep; jp++) {
                if (*jp != 0) {
                    /* kept, as in MSL's timesdec: break + a jp test (0x1b4) or an
                       inline predicate (0x1bc) both grow it */
                    goto round;
                }
            }
            if ((ip[-1] & 1) == 0) {
                return;
            }
        }
    round:
        func_02071644(result, result->sig.length);
    }
}


typedef struct Decimal
{
  unsigned char sign;
  char unused;
  short exp;
  struct
  {
    unsigned char length;
    unsigned char text[32];
    unsigned char unused;
  } sig;
} Decimal;
void func_02071510(Decimal *result, unsigned long long v)
{
  result->sign = 0;
  result->sig.length = 0;
  if (v != 0)
  {
    do
    {
      result->sig.text[result->sig.length++] = (unsigned char) (v % 10);
      v /= 10;
    }
    while (v != 0);
  }
  {
    unsigned char len = result->sig.length;
    unsigned char *s = result->sig.text;
    unsigned char *e = (s + len) - 1;
    while (s < e)
    {
      unsigned char t = *s;
      *s = *e;
      *e = t;
      s++;
      e--;
    }
  }
  result->exp = (short) (result->sig.length - 1);
}


#include "types.h"
#include "Decimal.h"

// func_02071698 is still spelled the way its own definition is, over a raw byte
// pointer and its own offsets; recovering that interface is a separate slice.
extern int func_02071698(void *value, int length);
extern void func_02071644(Decimal *value, int length);

void func_020715e0(Decimal *value, int length)
{
    int r;
    if (length <= 0) return;
    if (length >= value->sig.length) return;
    r = func_02071698(value, length);
    value->sig.length = length;
    if (r < 0) return;
    func_02071644(value, length);
}


// Increment the decimal significand, propagating carry into the exponent.
// The 2004/b56 output occupies 0x02071644..0x02071698, including the final bx lr.
// recovered: real C, no asm hatch; the investigation is in notes/mwccarm-codegen.md 9a(3).
#include "types.h"
#include "Decimal.h"

void func_02071644(Decimal *d, int len)
{
  unsigned char *base = d->sig.text;
  unsigned char *p = base + len - 1;
  for (;;)
  {
    unsigned char c = *p;
    if (c < 9)
    {
      *p = c + 1;
      return;
    }
    if (p == base)
    {
      *p = 1;
      d->exp++;
      return;
    }
    *p-- = 0;
  }
}


#include "types.h"
int func_02071698(u8* base, int idx)
{
    u8* p = base + 5 + idx;
    u8 e = *p;
    u8* end;
    if (e > 5) return 1;
    if (e < 5) return -1;
    p = p + 1;
    end = base + 5 + base[4];
    while (p < end) {
        if (*p != 0) return 1;
        p = p + 1;
    }
    return ((base + (idx - 1))[5] & 1) ? 1 : -1;
}


int func_02071708(int w0, ...)
{
    unsigned char *p;
    int count;
    unsigned int c;
    if (w0 != 0) {
        p = (unsigned char *)&w0;
        count = 0;
        c = *p;
        if (c == 0) {
            do {
                ++p;
                count += 8;
                c = *p;
            } while (c == 0);
        }
        c = ~c;
        *p = (unsigned char)c;
        if ((*(volatile unsigned char *)p & 1) == 0)
            return count;
        do {
            count++;
            *p = (unsigned char)((int)*(volatile unsigned char *)p >> 1);
        } while (*(volatile unsigned char *)p & 1);
        return count;
    }
    return 0x20;
}
