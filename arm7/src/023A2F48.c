/* storage 0x023A2F48..0x023A3714, runtime 0x027D4EC0..0x027D568C.
 * Complete extent: 0x78C bytes of code plus a 0x40-byte literal pool.
 * WM parent-request processing, reconstructed from the AH61 C draft by
 * codex-arm7-remaining-astra1014. The earlier 0x3C pool and allocator-limit
 * claims were incomplete; their failed experiments remain in the handoff.
 *
 * The local parser and frame types mirror the actual definitions in
 * 023A1828.c and 023A40DC.c; no shared ARM7 type header exists for them.
 * Register-based assembly helpers retain explicit C call interfaces.
 * Unknown work-area fields and the opaque record prefix remain partial.
 */
/* Partial observed table record: stride 0x1c, with halfwords at 0x18/0x1a.
 * The first 24 bytes remain uninterpreted; this is not an original type/name claim. */
struct PartialRecord {
    unsigned char unknown00[24];
    unsigned short field18;
    unsigned short field1a;
};
typedef char RecordSizeCheck[sizeof(struct PartialRecord) == 28 ? 1 : -1];
#define RECORD_TABLE (*(struct PartialRecord **)(*(char **)0x0380FFF4 + 0x2E0))
struct Bit0 { unsigned short b0:1; unsigned short rest:15; };
typedef union { unsigned long long value; unsigned short word[4]; } TickWords;
typedef struct {
    unsigned char *cur;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fA;
    unsigned short fC;
    unsigned short fE;
    unsigned short f10;
    unsigned short f12;
    unsigned short f14;
    unsigned short f16;
    unsigned char *f18;
    unsigned char *f1C;
    unsigned char *f20;
    unsigned char *f24;
} T;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef struct {
    u16 _a[4];
} FIRM_HEADER;

typedef union {
    struct {
        u16 _p[5];
        u16 rsv_RSSI;   /* +0x12 */
    } Rx;
} MAC_HEADER;

typedef struct {
    u16 FrameCtrl;      /* +0x00 */
    u16 Duration;       /* +0x02 */
    u16 DA[3];          /* +0x04 */
    u16 SA[3];          /* +0x0A */
    u16 BSSID[3];       /* +0x24 within the frame header layout used below */
    u16 SeqCtrl;
} MAN_HEADER;

typedef union {
    u16 Data;
    u16 Bit;
} CAPA_INFO;

typedef struct {
    u32 TimeStamp[2];   /* +0x00 */
    u16 BeaconInterval; /* +0x34 within the frame */
    CAPA_INFO CapaInfo; /* +0x36 */
    u8 Buf[54];
    u8 pad[2];
} BEACON_BODY;

typedef struct {
    FIRM_HEADER FirmHeader;
    MAC_HEADER MacHeader;
    MAN_HEADER Dot11Header;
    BEACON_BODY Body;
} BEACON_FRAME;

typedef struct {
    u8 ID;      /* +0x00 */
    u8 Length;  /* +0x01 */
    u8 SSID[32];/* +0x02 */
} SSID_ELEMENT;
extern int fn_0239AC04(unsigned short *a1); /* =0x027CCB7C */
extern int fn_0239A96C(int a1, unsigned short a2); /* =0x027CC8E4 */
extern int fn_02382CB4(int v, int *p, int n); /* =0x037FAB4C */
extern void fn_023A1828(T *a1); /* =0x027D37A0 */
extern unsigned char fn_02395334(void *addr); /* =0x027C72AC */
extern void fn_023A2344(unsigned short *a1, int *a2); /* =0x027D42BC */
extern void fn_023954C4(void); /* =0x027C743C */
extern int fn_023970C8(unsigned int a1, unsigned int a2); /* =0x027C9040 */
extern int fn_023979B0(unsigned short a1, int a2); /* =0x027C9928 */
extern int fn_02397468(unsigned short a1); /* =0x027C93E0 */
extern int fn_023974A8(unsigned short a1); /* =0x027C9420 */
extern unsigned short *fn_023969D8(unsigned short *result, unsigned short *a2); /* =0x027C8950 */
extern void fn_0239477C(int a1, int a2); /* =0x027C66F4 */
extern int fn_02396EFC(unsigned int a1); /* =0x027C8E74 */
extern unsigned long long fn_0238D38C(unsigned long long a1, unsigned long long a2); /* =0x03805224 */
extern int fn_02382AC4(void); /* =0x037FA95C */
extern int fn_02382AD8(int a1); /* =0x037FA970 */
extern void fn_02382C98(void *a1, void *a2, unsigned int a3); /* =0x037FAB30 */
extern void fn_0239F6D8(void); /* =0x027D1650 */
extern int fn_0239B25C(unsigned short *a1); /* =0x027CD1D4 */
extern void fn_023A40DC(u16 channel, BEACON_FRAME *pFrm, SSID_ELEMENT *pSSID); /* =0x027D6054 */

void fn_023A2F48(unsigned short *a1) {
    unsigned long long timestamp;
    unsigned short *pp;
    BEACON_BODY *body;
    TickWords sample1;
    TickWords sample2;
    /* Matched stack store/load, also used by the enrolled parser siblings.
     * This preserves the observed clear-call sequence, not a recovered SDK macro. */
    volatile int zero;
    T buf;
    int aid;
    unsigned int cnt;
    int v5;
    unsigned int v6;
    char *v7;
    unsigned int v8;
    int v18;
    unsigned long long v26;
    unsigned int v32;
    unsigned int firstBit;
    unsigned int v33;
    unsigned int v34;
    int v28;
    unsigned int v29;
    unsigned int v30;

    ++*(unsigned int *)((char *)*(char **)0x0380FFF4 + 0x528);
    aid = fn_0239AC04(a1 + 15);
    a1[1] = (unsigned short)aid;
    if (aid == 255)
        return;
    fn_0239A96C((unsigned short)aid, a1[9]);
    body = &((BEACON_FRAME *)a1)->Body;
    cnt = a1[3];
    if (cnt > 12) {
        zero = 0;
        fn_02382CB4(zero, (int *)&buf, sizeof(buf));
        buf.cur = body->Buf;
        buf.f8 = (unsigned short)(cnt - 12);
        buf.fA = 2;
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x1A) == 0)
            buf.fA |= 1;
        buf.fC = 56;
        buf.f4 = a1[4];
        buf.f6 = body->CapaInfo.Data;
        fn_023A1828(&buf);
        v5 = (int)buf.f1C;
        if (v5 != 0 && (a1[11] & 0x8000) != 0) {
            v6 = fn_02395334((void *)((unsigned int)(v5 + 6)));
            *(unsigned short *)0x480810C = (unsigned short)(v6 + (fn_02395334((void *)((unsigned int)(v5 + 7))) << 8));
        }
        v7 = *(char **)0x0380FFF4 + 776;
        v8 = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xBC);
        if (v8 == 19 && *(unsigned short *)(*(unsigned int *)(*(char **)0x0380FFF4 + 980) + 56) == 1) {
            if ((buf.fA & 9) == 9)
                fn_023A2344(a1, (int *)&buf);
            goto tail;
        }
        if ((buf.fA & 8) == 0)
            goto tail;
        if ((buf.fC & 1) != 0) {
            if ((buf.fC & 1) == 0)
                goto tail;
            if ((buf.fA & 1) == 0)
                goto tail;
        }
        if (v8 == 33) {
            fn_023954C4();
            if ((buf.fA & 0x30) != 0x30) {
                *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 10;
            } else {
                if ((buf.fA & 4) == 0) {
                    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                    *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 18;
                } else {
                    if (a1[26] > 0x3E8) {
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 12;
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC2) = 1;
                    } else {
                        *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xC0) = 0;
                        if ((buf.fC & 2) != 0 && (buf.fA & 2) == 0)
                            fn_023970C8(buf.f12, 0);
                        if (*(unsigned short *)(v7 + 12) == 2) {
                            if (((unsigned short *)buf.f24) != 0) {
                                fn_023979B0((unsigned short)(fn_02395334((void *)((unsigned int)(((unsigned short *)buf.f24) + 3))) + (fn_02395334((void *)((unsigned int)((char *)((unsigned short *)buf.f24) + 7))) << 8)), 1);
                                *(unsigned short *)0x380FFF0 = (unsigned short)(fn_02395334((void *)((unsigned int)(((unsigned short *)buf.f24) + 4))) + (fn_02395334((void *)((unsigned int)((char *)((unsigned short *)buf.f24) + 9))) << 8));
                            } else {
                                fn_023979B0(0xFFFF, 1);
                                *(unsigned short *)0x380FFF0 = 0;
                            }
                        }
                        fn_02397468(fn_02395334((void *)((unsigned int)((char *)buf.f20 + 3))));
                        *(unsigned short *)(v7 + 106) = (unsigned short)fn_02395334((void *)((unsigned int)((char *)buf.f20 + 2)));
                        fn_023974A8(a1[26]);
                        *(unsigned short *)(v7 + 14) |= 4;
                        *(unsigned short *)(v7 + 14) |= 0x40;
                        if (*(unsigned short *)(v7 + 12) == 2)
                            *(unsigned short *)0x4808048 = 3;
                        *(unsigned short *)0x4808038 |= 1;
                    }
                }
            }
            fn_023969D8((unsigned short *)(*(unsigned int *)(*(char **)0x0380FFF4 + 984) + 8), (unsigned short *)(a1 + 15));
            *(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0xBC) = 37;
            fn_0239477C(2, 1);
        }
        v18 = *(unsigned short *)(v7 + 12);
        switch (v18) {
        case 1:
        default:
            goto l65;
        case 2: {
            if (((unsigned short *)buf.f24) != 0) {
                fn_023979B0((unsigned short)(fn_02395334((void *)((unsigned int)(((unsigned short *)buf.f24) + 3))) + (fn_02395334((void *)((unsigned int)((char *)((unsigned short *)buf.f24) + 7))) << 8)), 0);
                *(unsigned short *)0x380FFF0 = (unsigned short)(fn_02395334((void *)((unsigned int)(((unsigned short *)buf.f24) + 4))) + (fn_02395334((void *)((unsigned int)((char *)((unsigned short *)buf.f24) + 9))) << 8));
                *(unsigned short *)(v7 + 148) = (unsigned short)(fn_02395334((void *)((unsigned int)((char *)((unsigned short *)buf.f24) + 1))) - 8);
                if (*(unsigned short *)(v7 + 148) != 0) {
                    if ((((unsigned int)((unsigned short *)buf.f24)) & 1) != 0) {
                        fn_02382C98((char *)((unsigned short *)buf.f24) + 9, *(void **)(v7 + 144), (unsigned int)(*(unsigned short *)(v7 + 148) + 2));
                        *(unsigned short *)(v7 + 150) = 1;
                    } else {
                        fn_02382C98(((unsigned short *)buf.f24) + 5, *(void **)(v7 + 144), (unsigned int)(*(unsigned short *)(v7 + 148) + 1));
                        *(unsigned short *)(v7 + 150) = 0;
                    }
                }
            }
            break;
        }
        case 3:
            break;
        }
        *(unsigned short *)(v7 + 116) = 0;
        {
            RECORD_TABLE[aid].field18 = RECORD_TABLE[aid].field1a;
        }
        timestamp = *(unsigned long long *)(a1 + 22);
        {
            char *w300 = (char *)*(char **)0x0380FFF4 + 0x300;
            v26 = (unsigned int)(*(unsigned short *)(w300 + 0x6A) << 10);
        }
        timestamp = fn_0238D38C(timestamp, v26);
        timestamp += 1;
        timestamp *= v26;
        pp = (unsigned short *)&timestamp;
        *(unsigned short *)0x48080F6 = pp[3];
        *(unsigned short *)0x48080F4 = pp[2];
        *(unsigned short *)0x48080F2 = pp[1];
        *(unsigned short *)0x48080F0 = pp[0] | 1;
        if (((unsigned int)*(unsigned short *)(v7 + 14) << 25) >> 31) {
            timestamp -= v26;
            v28 = fn_02382AC4();
            sample1.word[0] = *(volatile unsigned short *)0x48080F8;
            sample1.word[1] = *(volatile unsigned short *)0x48080FA;
            sample1.word[2] = *(volatile unsigned short *)0x48080FC;
            sample1.word[3] = *(volatile unsigned short *)0x48080FE;
            sample2.word[0] = *(volatile unsigned short *)0x48080F8;
            sample2.word[1] = *(volatile unsigned short *)0x48080FA;
            sample2.word[2] = *(volatile unsigned short *)0x48080FC;
            sample2.word[3] = *(volatile unsigned short *)0x48080FE;
            fn_02382AD8(v28);
            if (sample1.word[0] < sample2.word[0])
                v29 = (unsigned int)((sample1.value - timestamp) >> 10);
            else
                v29 = (unsigned int)((sample2.value - timestamp) >> 10);
            v30 = *(unsigned short *)(*(char **)0x0380FFF4 + 0x300);
            if (v29 < v30)
                *(unsigned short *)0x4808134 = (unsigned short)(v30 - v29);
            else
                *(unsigned short *)0x4808134 = 0;
        }
        if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x300 + 0x10) == 64 && (char *)buf.f20 != 0
            && ((struct Bit0 *)(*(char **)0x0380FFF4 + 0x316))->b0 == 1) {
            unsigned int v31 = fn_02395334((void *)((unsigned int)((char *)buf.f20 + 2)));
            if (*(unsigned short *)(v7 + 106) != v31)
                *(unsigned short *)(v7 + 106) = (unsigned short)v31;
            *(unsigned short *)(v7 + 130) = 0;
            if (v31 == 0 && (fn_02395334((void *)((unsigned int)((char *)buf.f20 + 4))) & 1) != 0)
                *(unsigned short *)(v7 + 130) |= 1;
            v32 = fn_02395334((void *)((unsigned int)((char *)buf.f20 + 4))) & 0xFE;
            firstBit = 8 * v32;
            v33 = 8 * (fn_02395334((void *)((unsigned int)((char *)buf.f20 + 1))) + v32 - 3);
            v34 = *(unsigned short *)(v7 + 94);
            if (firstBit <= v34 && v34 <= v33
                && ((1 << ((v34 - firstBit) & 7)) & fn_02395334((void *)((unsigned int)((char *)buf.f20 + 5 + ((v34 - firstBit) >> 3))))) != 0) {
                *(unsigned short *)(v7 + 130) |= 2;
                fn_0239F6D8();
            }
            if (*(unsigned short *)(*(char **)0x0380FFF4 + 0x100 + 0x60) == 0
                && *(unsigned short *)(*(char **)0x0380FFF4 + 0x100 + 0x6C) == 0
                && *(unsigned short *)(v7 + 130) == 0)
                fn_02396EFC(1);
        }
l65:
        if (((unsigned int)*(unsigned short *)(*(char **)0x0380FFF4 + 0x200 + 0xFE) << 25) >> 31)
            fn_0239B25C(a1);
tail:
        if (buf.f18 != 0)
            fn_023A40DC(buf.f12, (BEACON_FRAME *)a1, (SSID_ELEMENT *)buf.f18);
    }
}
