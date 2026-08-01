typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern void MulMat4x3Mat4x3(const void *m1, const void *m0, void *mF);
extern void func_02055998(int *m);
extern void func_02055388(int *m);
extern void func_020553a4(int *m);

extern int data_0209b3ec;
extern int data_ov080_0212771c;

void func_ov080_021261f4(char *self)
{
    int col0;
    int tmp[12];
    int row;
    int viewport;
    int col;

    col0 = 0;

    MulMat4x3Mat4x3(self + 0xd4, &data_0209b3ec, tmp);

    *(volatile u32 *)0x4000444 = 0;
    func_02055998((int *)&data_ov080_0212771c);

    func_02055388(tmp);

    *(volatile u32 *)0x4000440 = 2;
    func_020553a4(tmp);

    *(volatile u32 *)0x4000488 = 0x4000400;
    *(volatile u32 *)0x40004c8 = 0xe0000000;
    *(volatile u32 *)0x40004cc = 0xc0007fff;

    {
        u32 *obj = *(u32 **)(self + 0x1a8);
        *(volatile u32 *)0x40004c0 = obj[2] | 0x7fff0000;
    }
    {
        u32 *obj = *(u32 **)(self + 0x1a8);
        *(volatile u32 *)0x40004c4 = obj[3];
    }
    {
        u32 *obj = *(u32 **)(self + 0x1a8);
        u32 w = obj[4];
        u32 b = obj[0];
        u32 f0 = w >> 0x1a;
        u32 f1 = w >> 0x14;
        u32 f2 = w >> 0x17;
        u32 f3 = w >> 0x1d;
        *(volatile u32 *)0x40004a8 =
            (b >> 3)
            | ((f0 & 7) << 26)
            | 0x80000000
            | ((f1 & 7) << 20)
            | ((f2 & 7) << 23)
            | ((f3 & 1) << 29);
    }
    {
        u32 *obj = *(u32 **)(self + 0x1a8);
        u32 k = (obj[4] >> 0x1a) & 7;
        *(volatile u32 *)0x40004ac = obj[1] >> (4 - (k == 2 ? 1 : 0));
    }
    {
        u8 v = (*(u32 *)(self + 8) >> 8) & 0x1f;
        *(volatile u32 *)0x40004a4 = 0x1000088 | ((v == 7 ? 0x14 : 0x1f) << 16);
    }

    *(volatile u32 *)0x400046c = 0x10000;
    *(volatile u32 *)0x400046c = 0x10000;
    *(volatile u32 *)0x400046c = 0x10000;

    row = 0;
    if ((int)*(u8 *)(self + 0x1ba) - 1 > 0) {
        volatile u32 *pNormal = (volatile u32 *)0x4000484;
        volatile u32 *pVtx16 = (volatile u32 *)0x400048c;
        viewport = row;
        col0 = row;
        do {
            *(volatile u32 *)0x4000500 = 2;
            col = col0;
            if ((int)*(u8 *)(self + 0x1bb) > 0) {
                do {
                    char *verts = *(char **)(self + 0x1a0);
                    int n = col * (*(u8 *)(self + 0x1bb));
                    int off0 = (row + n) * 0x18;
                    int off1 = (row + 1 + n) * 0x18;
                    char *p0 = verts + off0;
                    char *p1 = verts + off1;
                    int x, y, z;
                    int xsh, zsh;
                    u16 sx, sy, sz;

                    *pNormal = *(u32 *)(p0 + 0x10);
                    col++;
                    x = *(int *)(verts + off0);
                    z = *(int *)(p0 + 8);
                    xsh = x << 9;
                    zsh = z << 9;
                    zsh = (int)zsh >> 16;
                    xsh = (int)xsh >> 16;
                    sx = (u16)xsh;
                    y = *(int *)(p0 + 4);
                    zsh = zsh << 16;
                    sy = (u16)((int)(y << 9) >> 16);
                    *pVtx16 = sx | (sy << 16);
                    *pVtx16 = (unsigned)zsh >> 16;

                    *pNormal = *(u32 *)(p1 + 0x10);
                    x = *(int *)(verts + off1);
                    z = *(volatile int *)(p1 + 8);
                    xsh = x << 9;
                    zsh = z << 9;
                    zsh = (int)zsh >> 16;
                    xsh = (int)xsh >> 16;
                    sx = (u16)xsh;
                    y = *(int *)(p1 + 4);
                    zsh = zsh << 16;
                    sy = (u16)((int)(y << 9) >> 16);
                    *pVtx16 = sx | (sy << 16);
                    *pVtx16 = (unsigned)zsh >> 16;
                } while (col < *(u8 *)(self + 0x1bb));
            }
            *(volatile u32 *)0x4000504 = viewport;
            row++;
        } while (row < (int)*(u8 *)(self + 0x1ba) - 1);
    }

    *(volatile u32 *)0x4000448 = 1;
}
