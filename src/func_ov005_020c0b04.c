// NONMATCHING: init r0/r1 + store order; loop-inc order; RenderSub arg setup (div=26).
// Size matches. Improved from div=32 via tmp reuse + outer loop y1 schedule.
struct Obj {
    char pad0[0x54];
    unsigned char unk54;
    char pad1[3];
    int unk58;
    char pad2[0x8c - 0x5c];
    int unk8C;
    int unk90;
    int unk94;
    int unk98;
    int unk9C;
};

extern unsigned char data_0209b300;
extern unsigned char data_0209b304;
extern int data_ov005_020c2250[];
extern int data_ov005_020c2c28[];
extern int data_ov005_020c2e88;
extern int data_ov005_020c2ea4[];
extern int data_ov005_020c2f60[];
extern int data_ov005_020c2f88;
extern int data_ov005_020c2fcc;

extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int, int, int, int, int, int, int, int, int, int);
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int, int, int, int, int, int, int, int);
extern int _ZN3OAM9RenderSubEP7OamAttriiii(int, int, int, int, int);
extern int func_ov005_020c00b4(void *, int);
extern int func_ov005_020c007c(void *, int);
extern int func_ov005_020c00e4(void *);
extern void func_ov005_020c1130(void *);
extern void func_ov005_020c0f38(void *);

int func_ov005_020c0b04(struct Obj *arg0)
{
    volatile int v[10];
    int sb, fp, r8, r7, r6, r5, r4;
    int t, t2, scale;
    int tmp;

    if (data_0209b300 == 1) {
        tmp = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2f88, 0x80, 0xA4, -1, 0, 0x1000, 0x1000, 0, -1);
    } else {
        tmp = _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov005_020c2fcc, 0x80, 0xA4, -1, 0, 0x1000, 0x1000, 0, -1);
    }

    /* reuse return reg path: overwrite tmp with 0x18 like ROM reuses r0 */
    tmp = 0x18;
    v[1] = tmp;
    sb = 0;
    v[6] = tmp;
    v[9] = tmp;
    tmp = 1;
    v[2] = tmp;
    tmp = -1;
    fp = 0x28;
    v[3] = sb;
    v[4] = sb;
    v[5] = sb;
    v[8] = sb;
    v[7] = tmp;

    do {
        r7 = v[4];
        r8 = sb;
        r5 = r7;
        if (arg0->unk9C >= 0x20)
            v[0] = v[2];
        else
            v[0] = v[3];
        r4 = data_ov005_020c2250[sb];
        r6 = r4;
        do {
            if (func_ov005_020c00b4(arg0, r6) != 0) {
                if (func_ov005_020c007c(arg0, r5 + r4) != 0) {
                    _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2f60[v[0]], v[6], fp, v[7], v[5]);
                    goto next_outer;
                }
            }
            r6 += 4;
            r5 += 4;
            r7 += 1;
        } while (r7 < 9);
    next_outer:
        {
            int y9 = v[9];
            int x8 = v[8];
            if (arg0->unk58 != r4)
                r8 += 4;
            _ZN3OAM9RenderSubEP7OamAttriiii(data_ov005_020c2c28[r8], y9, v[1], v[7], x8);
        }
        {
            int y1 = v[1];
            sb += 1;
            y1 += 0x30;
            fp += 0x30;
            v[1] = y1;
        }
    } while (sb < 4);

    if (arg0->unk54 == 0) {
        if (data_0209b304 != 0) {
            t = arg0->unk90;
            if (t < 0xA)
                scale = 0x1000;
            else if (t < 0xE)
                scale = (((t - 0xA) << 0xC) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 0xC) / 16) + 0x1000;
            t2 = arg0->unk8C;
            if (t2 < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - t2 / 2, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[0], 0x50 - (0x40 - t2) / 2, 0x60, -1, 0, scale, 0);
        } else if (func_ov005_020c00e4(arg0) != 0) {
            t = arg0->unk94;
            if (t < 0xA)
                scale = 0x1000;
            else if (t < 0xE)
                scale = (((t - 0xA) << 0xC) / 16) + 0x1000;
            else
                scale = (((0x12 - t) << 0xC) / 16) + 0x1000;
            t2 = arg0->unk8C;
            if (t2 < 0x20)
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], t2 / 2 + 0xE0, 0x60, -1, 0, scale, 0);
            else
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2ea4[1], (0x40 - t2) / 2 + 0xE0, 0x60, -1, 0, scale, 0);
        }
    }

    func_ov005_020c1130(arg0);
    func_ov005_020c0f38(arg0);

    t = arg0->unk98;
    if (t < 0x14)
        scale = 0x1000;
    else if (t < 0x18)
        scale = (((t - 0x14) << 0xC) / 16) + 0x1000;
    else
        scale = (((0x1C - t) << 0xC) / 16) + 0x1000;
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(1, data_ov005_020c2e88, 0xE8, 0xA0, -1, 0, scale, 0);
    return 1;
}
