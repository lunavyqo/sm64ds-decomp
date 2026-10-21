typedef struct
{
    unsigned char pad[0x14];
    unsigned int lenHi;
    unsigned int lenLo;
    int count;
    unsigned char buf[0x40];
} HashCtx;

extern void func_0205e3d4(HashCtx* c);

#define CNT (*(int*)(int)((char*)c + 0x1c))

void func_0205e280(HashCtx* c)
{
    if (c->count > 0x37)
    {
        int n = c->count;
        CNT++;
        c->buf[n] = 0x80;
        while (c->count < 0x40)
        {
            c->buf[c->count++] = 0;
        }
        func_0205e3d4(c);
        while (c->count < 0x38)
        {
            c->buf[c->count++] = 0;
        }
    }
    else
    {
        int n = c->count;
        CNT++;
        c->buf[n] = 0x80;
        while (c->count < 0x38)
        {
            c->buf[c->count++] = 0;
        }
    }

    c->buf[0x38] = c->lenLo >> 24;
    c->buf[0x39] = c->lenLo >> 16;
    c->buf[0x3a] = c->lenLo >> 8;
    c->buf[0x3b] = c->lenLo;
    c->buf[0x3c] = c->lenHi >> 24;
    c->buf[0x3d] = c->lenHi >> 16;
    c->buf[0x3e] = c->lenHi >> 8;
    c->buf[0x3f] = c->lenHi;

    func_0205e3d4(c);
}

/* SHA-1 block transform (RFC 3174 SHA1ProcessMessageBlock), plus a
   game-specific patch that temporarily zeroes two words of the message
   block while data_020a80c4 is set.

   Matching notes:
   - RFC 3174 declaration order is load-bearing: K, t, temp, W, A..E.
     `temp` lives at function scope, not inside the round loops, and the
     circular shift is the inline macro (no named rotate local). Block-scoped
     locals form their own late rank class and rotate the whole callee-saved
     assignment; hoisting them to the top is what lands the ROM coloring.
   - `save[2]` must NOT be volatile. Volatile stores are high-priority for the
     list scheduler and sink to the block front, which hoists the shared
     `mov r0, #0` above the two save loads.
   - The round-1 K read goes through a volatile pointer. Register pressure in
     the round-1 body is what keeps K[0] un-hoisted in the ROM; rounds 2..4
     hoist their K into the preheader on their own. */

typedef struct {
    unsigned int w[4];
} T4;

typedef struct {
    unsigned int Intermediate_Hash[5]; /* 0x00 */
    unsigned int Length_Low;           /* 0x14 */
    unsigned int Length_High;          /* 0x18 */
    unsigned int Message_Block_Index;  /* 0x1c */
    unsigned char Message_Block[64];   /* 0x20 */
} SHA1Context;

extern int data_020a80c4;
extern unsigned int data_0209a054[4];

#define SHA1CircularShift(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

void func_0205e3d4(void *ctx) {
    SHA1Context *context = (SHA1Context *)ctx;
    unsigned int K[4];
    int t;
    unsigned int temp;
    unsigned int W[80];
    unsigned int A, B, C, D, E;
    unsigned int save[2];
    unsigned char *blk;

    *(T4 *)K = *(T4 *)data_0209a054;
    blk = context->Message_Block;
    if (data_020a80c4 != 0) {
        save[0] = *(unsigned int *)(blk + 0x18);
        save[1] = *(unsigned int *)(blk + 0x38);
        *(unsigned int *)(blk + 0x18) = 0;
        *(unsigned int *)(blk + 0x38) = 0;
    }

    for (t = 0; t < 16; t++) {
        W[t] = (unsigned int)context->Message_Block[t * 4] << 24;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 1] << 16;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 2] << 8;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 3];
    }

    for (t = 16; t < 80; t++) {
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
    }

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for (t = 0; t < 20; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] +
               ((volatile unsigned int *)K)[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    *(unsigned int *)(((int)&context->Intermediate_Hash[1]) & ~0ULL) += B;
    *(unsigned int *)(((int)&context->Intermediate_Hash[2]) & ~0ULL) += C;
    *(unsigned int *)(((int)&context->Intermediate_Hash[3]) & ~0ULL) += D;
    *(unsigned int *)(((int)&context->Intermediate_Hash[4]) & ~0ULL) += E;
    context->Message_Block_Index = 0;

    if (data_020a80c4 != 0) {
        *(unsigned int *)(blk + 0x18) = save[0];
        *(unsigned int *)(blk + 0x38) = save[1];
    }
}

#include "types.h"
#pragma opt_loop_invariants off
struct Obj {
    char pad00[0x14];
    int f14;      // 0x14
    int f18;      // 0x18
    int f1c;      // 0x1c
    u8 buf[0x40]; // 0x20
    int f60;      // 0x60
    int f64;      // 0x64
};

extern void func_0205e3d4(struct Obj *);

int func_0205e6e4(struct Obj *obj, u8 *p, int count) {
    if (count == 0)
        return 0;
    if (obj == 0 || p == 0)
        return 1;
    if (obj->f60 != 0) {
        obj->f64 = 3;
        return 3;
    }
    if (obj->f64 != 0)
        return obj->f64;
    while (count-- != 0 && obj->f64 == 0) {
        int i = obj->f1c;
        *(int *)(((int)obj + 0x1c)) += 1;
        obj->buf[i] = *p;
        *(int *)(((int)obj + 0x14)) += 8;
        if (obj->f14 == 0) {
            *(int *)(((int)obj + 0x18)) += 1;
            if (obj->f18 == 0)
                obj->f64 = 1;
        }
        if (obj->f1c == 0x40)
            func_0205e3d4(obj);
        p++;
    }
    return 0;
}

extern int func_0205e280(char *c);

int func_0205e7e0(unsigned int *thiz, unsigned char *out) {
    int i;
    unsigned int r;
    if (thiz == 0 || out == 0)
        return 1;
    r = thiz[0x19];
    if (r != 0)
        return r;
    if (thiz[0x18] == 0) {
        char *p = (char *)thiz;
        func_0205e280(p);
        for (i = 0; i < 0x40; i++)
            (p + i)[0x20] = 0;
        thiz[5] = 0;
        thiz[6] = 0;
        thiz[0x18] = 1;
    }
    for (i = 0; i < 0x14; i++) {
        unsigned int idx = i >> 2;
        int shift = (3 - (i & 3)) * 8;
        out[i] = (unsigned char)(thiz[idx] >> shift);
    }
    return 0;
}
