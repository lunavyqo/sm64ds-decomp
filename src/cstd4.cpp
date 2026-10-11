//cpp
/* cstd and CP15 runtime helpers, 0x0206da64..0x0206e330.
   func_0206ce20 ends at 0x0206ce90 and the next enrolled function is this
   run, so it is not the cstd3 file. func_0206e330 is incomplete and is not
   taken. CP15::MPUDataRegion7 and CP15::MPUGetDataRegion7 stay CP15 methods
   in this file. Whole-function asm is emitted in source order; compiled
   bodies are too, under a per-function defer_codegen off. */
#include "types.h"

extern "C" {
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// @symbol func_0206da64
asm int func_0206da64(void) {
    mov r0, #1
    b lbl
    mov r0, #0
    b lbl
lbl:
    mrc p15, 0, r1, c1, c0, 0
    and r2, r1, #1
    tst r0, r0
    biceq r1, r1, #1
    orrne r1, r1, #1
    mcr p15, 0, r1, c1, c0, 0
    mov r0, r2
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// @symbol func_0206da94
asm void func_0206da94(void) { mrc p15, 0, r0, c7, c10, 1; bx lr }

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (SDK/runtime primitive: block copy, matrix/math, CP15, context
// switch, etc.), so there is no C to decompile it to -- the asm block is the
// faithful source. Counts as matched (asm-primitive policy), not a C transcription.
// @symbol func_0206da9c
asm void func_0206da9c(void) { mrc p15, 0, r0, c7, c6, 1; bx lr }
}

#pragma push
#pragma defer_codegen off
// @symbol _ZN4CP1517MPUGetDataRegion7Ev
/* CP15::MPUGetDataRegion7() at 0x0206daa4 -- read back MPU data region 7.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

unsigned int MPUGetDataRegion7(void)
{
    unsigned int v;
    asm { mrc p15,0,v,c6,c7,0 }
    return v;
}

}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol _ZN4CP1514MPUDataRegion7Ej
/* CP15::MPUDataRegion7(u32) at 0x0206daac -- write MPU data region 7 (c6,c7).
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. The asm-primitive policy is unchanged
 * by this migration; what changes is only that the compiler mangles the symbol
 * instead of the file spelling it by hand. Layout-free (plan phase 1): no
 * `this`, no vtable, no struct, no includers. */
namespace CP15 {

void MPUDataRegion7(unsigned int x)
{
    asm { mcr p15,0,x,c6,c7,0 }
}

}
#pragma pop

extern "C" {
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. The `swp` atomic swap has no C
// equivalent under mwccarm (predates C11 _Atomic), so this was assembly in the original --
// the asm block is the faithful source. Counts as matched (asm-primitive policy).
// @symbol func_0206dab4
asm void func_0206dab4(void) { swp r0, r0, [r1]; bx lr }
}

#pragma push
#pragma defer_codegen off
// @symbol _ZN4cstd14__builtin_trapEv
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).
//
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This function was assembly
// in the original (compiler trap builtin), so there is no C to decompile it to
// -- the asm block is the faithful source.

namespace cstd {

void __builtin_trap(void)
{
  asm { dcd 0xe7ffffff }
}

}
#pragma pop

extern "C" {
#pragma push
#pragma defer_codegen off
// @symbol func_0206dac4
extern int func_020589d4(void *p);
extern void func_02058b08(void *p);
extern void func_02058a94(void *p);
typedef struct S6e06c S6e06c;
int func_0206e06c(S6e06c *p);
extern void func_02071784(void);

extern int data_020aa020;
extern int data_020a6134[];
extern int data_020a9fd8;
extern int data_020a9ffc;
extern volatile int data_020a9ed0;
extern void (*volatile data_020a9ed8[])(void);
extern void (*data_020a9ec8)(void);

int func_0206dac4(int a)
{
    if (func_020589d4(&data_020aa020) == 0) {
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    } else if (data_020a9fd8 == *(int *)((char *)data_020a6134[2] + 0x6c)) {
        data_020a9ffc++;
    } else {
        func_02058b08(&data_020aa020);
        data_020a9fd8 = *(int *)((char *)data_020a6134[2] + 0x6c);
        data_020a9ffc = 1;
    }

    while (data_020a9ed0 > 0) {
        int n = data_020a9ed0 - 1;
        data_020a9ed0 = n;
        data_020a9ed8[n]();
    }

    data_020a9ffc--;
    if (data_020a9ffc == 0) {
        func_02058a94(&data_020aa020);
    }
    if (data_020a9ec8 != 0) {
        data_020a9ec8();
        data_020a9ec8 = 0;
    }
    func_0206e06c(0);
    func_02071784();
}
#pragma pop

#pragma push
#pragma defer_codegen off
typedef void (*FP0206dbf8)(void);

extern int data_020a9ed4;
extern FP0206dbf8 data_020a9ecc;
void func_02072f3c(void);

// @symbol func_0206dbf8
int func_0206dbf8(int a) {
    if (data_020a9ed4 == 0) {
        func_02072f3c();
        if (data_020a9ecc != 0) {
            data_020a9ecc();
            data_020a9ecc = 0;
        }
    }
    return func_0206dac4(a);
}
#pragma pop

#pragma push
#pragma defer_codegen off
void func_0207037c(int a);
extern int data_020a9ed4;

// @symbol func_0206dc4c
void func_0206dc4c(void) {
    func_0207037c(1);
    data_020a9ed4 = 1;
    func_0206dbf8(1);
}
#pragma pop

typedef struct Pair { int x, y; } Pair;
void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags);

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: double OP double -> float wrapper forwarding to the per-thread
// FP-operation dispatcher func_0206de14. Ships as ADS-era library code (below-sp
// argument staging with NO sp adjustment) that no mwccarm build/flags reproduces
// from C. Per asm policy; sibling of matched func_0206da64 in the same library.

// @symbol func_0206dc7c
asm float func_0206dc7c(double a, double b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    ldr ip, [sp, #0x28]
    str r2, [sp, #0x10]
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r3, [sp, #0x14]
    str ip, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: double OP double -> double wrapper forwarding to the per-thread
// FP-operation dispatcher func_0206de14. Ships as ADS-era library code (below-sp
// argument staging with NO sp adjustment) that no mwccarm build/flags reproduces
// from C. Per asm policy; sibling of matched func_0206da64 in the same library.

// @symbol func_0206dcd4
asm double func_0206dcd4(double a, double b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    ldr ip, [sp, #0x28]
    str r2, [sp, #0x10]
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r3, [sp, #0x14]
    str ip, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    ldr r1, [sp, #0x1c]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: float OP float -> float wrapper (floats widened in-place into
// uninitialized double slots) forwarding to the per-thread FP-operation
// dispatcher func_0206de14. Ships as ADS-era library code (below-sp argument
// staging with NO sp adjustment) that no mwccarm build/flags reproduces from C.
// Per asm policy; sibling of matched func_0206da64 in the same library.

// @symbol func_0206dd30
asm float func_0206dd30(float a, float b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    str r0, [sp, #8]
    str r1, [sp, #0x10]
    str r2, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: unary double -> double wrapper (operand duplicated into both
// dispatcher slots) forwarding to the per-thread FP-operation dispatcher
// func_0206de14. Ships as ADS-era library code (below-sp argument staging with
// NO sp adjustment) that no mwccarm build/flags reproduces from C. Per asm
// policy; sibling of matched func_0206da64 in the same library.

// @symbol func_0206dd7c
asm double func_0206dd7c(double a, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x1c
    str r0, [sp, #8]
    str r1, [sp, #0xc]
    str r2, [sp, #4]
    ldr r1, [sp, #8]
    ldr r0, [sp, #0xc]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x10
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x10]
    ldr r1, [sp, #0x14]
    add sp, sp, #0x1c
    ldmia sp!, {lr}
    bx lr
}

// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: unary float -> float wrapper (float widened in-place into an
// uninitialized double slot, duplicated into both dispatcher operand slots)
// forwarding to the per-thread FP-operation dispatcher func_0206de14. Ships as
// ADS-era library code (below-sp argument staging with NO sp adjustment) that
// no mwccarm build/flags reproduces from C. Per asm policy; sibling of matched
// func_0206da64 in the same library.

// @symbol func_0206ddcc
asm float func_0206ddcc(float a, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x1c
    str r0, [sp, #8]
    str r1, [sp, #4]
    ldr r1, [sp, #8]
    ldr r0, [sp, #0xc]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x10
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x10]
    add sp, sp, #0x1c
    ldmia sp!, {lr}
    bx lr
}

#pragma push
#pragma defer_codegen off
/* Dispatches to one of five handlers selected by bits 26-30 of flags, then
 * forwards its own arguments unchanged.
 *
 * The two 8-byte struct parameters are load-bearing, not cosmetic. p2 straddles
 * the r3/stack boundary, so the forwarded call has to stage it contiguously
 * across that edge: the ROM writes the pair to [sp-4] and [sp], then reads the
 * register half back with `ldm r2, {r3}`. Modelling the arguments as five ints
 * read through &a0 produces plain per-slot loads instead and is 12 bytes short.
 *
 * Needs mwccarm 2004/b56. Every 1.2 and 2.0 build brackets that below-sp staging
 * with `mov sp, r2` / `add sp, r2, #4` and takes a frame pointer to do it, which
 * is the same pre-2005 codegen difference recorded in notes/mwccarm-codegen.md
 * 6ai for the fBase_c::Process wrappers.
 */
extern int func_02073238(void);
extern int func_01ffb008(int a, int b);
extern void func_0207037c(int idx);

typedef void (*DispatchFn)(int a0, Pair p1, Pair p2, unsigned int flags);

// @symbol func_0206de14
void func_0206de14(int a0, Pair p1, Pair p2, unsigned int flags)
{
  DispatchFn fn = (DispatchFn) func_02073238();
  if ((flags & 0xc00000) == 0)
  {
    flags |= func_01ffb008(0, 0) & 0xc00000;
  }
  switch (flags & 0x7c000000)
  {
    case 0x40000000:
      fn = *((DispatchFn *) (((char *) fn) + 0x14));
      break;

    case 0x04000000:
      fn = *((DispatchFn *) (((char *) fn) + 4));
      break;

    case 0x10000000:
      fn = *((DispatchFn *) (((char *) fn) + 0xc));
      break;

    case 0x20000000:
      fn = *((DispatchFn *) (((char *) fn) + 0x10));
      break;

    case 0x08000000:
      fn = *((DispatchFn *) (((char *) fn) + 8));
      break;

    default:
      fn = 0;
      break;

  }

  if (fn == 0)
  {
    func_0207037c(2);
  }
  fn(a0, p1, p2, flags);
}
#pragma pop

#pragma push
#pragma defer_codegen off
/* func_0206df14 at 0x0206df14
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern char data_0209a330[];
typedef struct S6e06c S6e06c;
int func_0206e06c(S6e06c *p);

// @symbol func_0206df14
int func_0206df14(void) {
    int result = 0;
    char *p = data_0209a330;
    int i = 1;
    do {
        if (((unsigned int)*(int *)(p + 4) << 22) >> 29) {
            if (func_0206e06c((S6e06c *)p)) {
                result = -1;
            }
        }
        p = (i < 3) ? (data_0209a330 + i++ * 0x4c) : 0;
    } while (p);
    return result;
}
#pragma pop
}

#pragma push
#pragma defer_codegen off
// @symbol _ZN4cstd3absEi
//
// Language-mode flip only: the compiler mangles the name, it is no longer
// spelled by hand. Signature and body preserved exactly; no codegen intent.
// See notes/plan-cpp-language-mode.md phase 1 (layout-free SDK namespaces).


namespace cstd {

int abs(int x)
{
  return x<0 ? -x : x;
}

}
#pragma pop

extern "C" {
#pragma push
#pragma defer_codegen off
void func_0206e068(int a, int *b);
void func_0206e030(char *self);
typedef int (*Fn)(int, int, int *, int);
// @symbol func_0206df90
int func_0206df90(char *thiz, int *out)
{
  char *new_var;
  int diff = (*((int *) (thiz + 0x24))) - (*((int *) (thiz + 0x1c)));
  if (diff != 0)
  {
    int r;
    *((int *) (thiz + 0x28)) = diff;
    if (((((unsigned int) ((*((unsigned int *) (thiz + 4))) << 0x13)) >> 9) >> 22) == 0)
    {
      func_0206e068(*((int *) (thiz + 0x1c)), (int *) (thiz + 0x28));
    }
    r = (*((Fn *) (thiz + 0x40)))(*((int *) thiz), *((int *) (thiz + 0x1c)), (int *) (thiz + 0x28), *((int *) (thiz + 0x48)));
    if (out != 0)
    {
      *out = *((int *) (thiz + 0x28));
    }
    if (r != 0)
    {
      return r;
    }
    new_var = thiz + 0x18;
    *(int *)(new_var) += *((int *) (thiz + 0x28));
  }
  func_0206e030(thiz);
  return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e030
void func_0206e030(char *self)
{
    *(int *)(self + 0x24) = *(int *)(self + 0x1c);
    *(int *)(self + 0x28) = *(int *)(self + 0x20);
    *(int *)(self + 0x28) -=
        (*(int *)(self + 0x18) & *(int *)(self + 0x2c));
    *(int *)(self + 0x34) = *(int *)(self + 0x18);
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e068
void func_0206e068(int a, int *b)
{
}
#pragma pop

#pragma push
#pragma defer_codegen off
typedef struct S6e06c {
    int unk0;
    unsigned int b4a:2;
    unsigned int b4b:3;
    unsigned int b4gap:2;
    unsigned int b4c:3;
    unsigned int b4rest:22;
    unsigned int b8:3;
    unsigned int b8rest:29;
    unsigned char fc;
    unsigned char fd;
} S6e06c;


// @symbol func_0206e06c
int func_0206e06c(S6e06c *p)
{
    if (p == 0) {
        return func_0206df14();
    }

    if (p->fd != 0 || p->b4c == 0)
        return -1;

    if (p->b4b == 1)
        return 0;

    if (p->b8 >= 3)
        *(unsigned int *)(((int)p + 8)) = (*(unsigned int *)(((int)p + 8)) & ~7) | 2;

    if (p->b8 == 2)
        *(int *)((char *)p + 0x28) = 0;

    if (p->b8 != 1) {
        *(unsigned int *)(((int)p + 8)) &= ~7;
        return 0;
    }

    if (func_0206df90((char *)p, 0) != 0) {
        p->fd = 1;
        *(int *)((char *)p + 0x28) = 0;
        return -1;
    }
    *(unsigned int *)(((int)p + 8)) &= ~7;
    *(int *)((char *)p + 0x18) = 0;
    *(int *)((char *)p + 0x28) = 0;
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
int func_0206e218(int a, int b);
extern void _ZN4cstd7strncpyEPcPKcj(char *d, const char *s, u32 n);

// @symbol func_0206e184
u32 func_0206e184(char *dst, u16 *src, u32 max)
{
    u32 total = 0;
    char buf[4];
    char *bp;
    if (dst == 0 || src == 0)
        return 0;
    bp = buf;
    for (;;) {
        u16 code = *src;
        int len;
        if (code == 0) {
            dst[total] = 0;
            break;
        }
        src++;
        len = func_0206e218((int)bp, code);
        if (total + len > max)
            break;
        _ZN4cstd7strncpyEPcPKcj(dst + total, bp, len);
        total += len;
        if (total > max)
            break;
    }
    return total;
}
#pragma pop

#pragma push
#pragma defer_codegen off
extern void *data_0209a438[];

// @symbol func_0206e218
int func_0206e218(int a, int b) {
    void **p = (void **)data_0209a438[2];
    ((void (*)(int, int))p[1])(a, b);
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e240
int func_0206e240(unsigned char *p, unsigned char v) {
    if (p == 0) return 0;
    *p = v;
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e254
int func_0206e254(unsigned short *dst, unsigned char *src, int flag) {
    if (src == 0) return 0;
    if (flag == 0) return -1;
    if (dst != 0) *dst = *src;
    if (*src == 0) return 0;
    return 1;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e28c
int func_0206e28c(const u8* a, const u8* b, u32 n) {
    while (n != 0) {
        u8 ca = *a++;
        u8 cb = *b++;
        if (ca != cb) {
            if (a[-1] < b[-1]) return -1;
            return 1;
        }
        n--;
    }
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e2cc
u8 *func_0206e2cc(u8 *p, int c, unsigned int n)
{
    u8 ch = (u8)c;
    if (n != 0) {
        do {
            if (*p++ == ch) return p - 1;
        } while (--n != 0);
    }
    return 0;
}
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e2f8
/* recovered: MSL memset -- the thin wrapper over __fill_mem.
 *
 * The return type is `void *`, not `int`. func_0206e330, already recovered in
 * src/unnamed/arm9/0206/func_0206e330.c as the MSL byte-head / 32-byte-block / word-tail fill
 * loop, is __fill_mem(void *dst, int val, u32 n); this function forwards all
 * three arguments to it unchanged and returns its own first argument, which is
 * exactly memset's contract. The one caller in the tree, _ZN7fBase_cnwEj,
 * calls it as func_0206e2f8(p, 0, size) with p a void *.
 *
 * include/decl_common.h carried `int` here after the bulk correction in
 * 2bcc1b99c ("32 rows declared void over a value"), which established only
 * that r0 holds a value and picked int as the generic value type. That is
 * refined to the pointer the body actually returns. Both spellings are one
 * word in r0, so the row is byte-neutral either way.
 */
extern void func_0206e330(void *dst, int val, unsigned int n);
void *func_0206e2f8(void *dst, int val, unsigned int n) { func_0206e330(dst, val, n); return dst; }
#pragma pop

#pragma push
#pragma defer_codegen off
// @symbol func_0206e310
void *func_0206e310(void *dst, const void *src, unsigned int n)
{
    unsigned char *d = (unsigned char *)dst;
    const unsigned char *s = (const unsigned char *)src;
    while (n != 0) {
        *d++ = *s++;
        n--;
    }
    return dst;
}
#pragma pop
}
