// NONMATCHING: pure regperm (c r4↔r6, unlocked r5↔r4, e40/de8 r6/r7↔r7/r4); size-exact 0x398, 0 structural (54 words)
extern void func_02012790(int a);
extern int _ZN8SaveData19IsCharacterUnlockedEj(unsigned int i);
extern int func_ov003_020adec0(char *c, unsigned int r6);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
extern unsigned char data_0209caa0[];
extern unsigned char data_02092128;
extern unsigned char data_02092114;
extern unsigned short data_020a0e58;
extern unsigned short data_020a0e5a[];

void func_ov003_020ae358(char *c)
{
    int unlocked;
    unsigned char *p_de8;
    unsigned char *p_e40;
    int i;
    unsigned char idx;
    unsigned char cur;

    idx = data_020a0e40;
    unlocked = 0;
    if (data_020a0de8[idx << 2] != 0)
        unlocked = (data_020a0de9[idx << 2] != 0);
    if (unlocked == 0)
        goto sect2;

    if ((unsigned char)(data_020a0dea[idx << 2] - 0x58) < 0x50u
        && (unsigned char)(data_020a0deb[idx << 2] - *(unsigned char *)(c + 0x12b) + 0x28) < 0x50u) {
        *(unsigned char *)(c + 0x133) = 2;
        *(unsigned char *)(c + 0x132) = 3;
        *(unsigned char *)(c + 0x118) = (unsigned char)(data_0208ee44 * 6);
        *(unsigned char *)(c + 0x139) = 1;
        *(unsigned char *)(c + 0x119) = 0x10;
        func_02012790(data_0209caa0[0x41] + 0x3c);
        return;
    }
    if (*(unsigned char *)(c + 0x130) <= 1u)
        return;
    if (data_0209caa0[0x41] != 3)
        return;

    p_e40 = &data_020a0e40;
    p_de8 = data_020a0de8;
    for (i = 0; i < 3; i++) {
        if (_ZN8SaveData19IsCharacterUnlockedEj(i) != 0) {
            unsigned char id2 = *p_e40;
            unsigned char *row = p_de8 + (id2 << 2);
            unsigned int dx = (unsigned short)(row[2] - *(unsigned char *)(c + 0x124 + i) + 0x18);
            if (dx < 0x30u) {
                unsigned int dy = (unsigned short)(row[3] - *(unsigned char *)(c + 0x128 + i) + 0x18);
                if (dy < 0x2bu) {
                    *(unsigned char *)(c + 0x133) = 1;
                    *(unsigned char *)(c + 0x134) = (unsigned char)func_ov003_020adec0(c, i);
                    data_02092128 = (unsigned char)i;
                    data_02092114 = (unsigned char)i;
                    *(unsigned char *)(c + 0x132) = (unsigned char)i;
                    *(unsigned char *)(c + 0x118) = (unsigned char)(data_0208ee44 * 3);
                    *(unsigned char *)(c + 0x139) = 2;
                    *(unsigned char *)(c + 0x119) = 0x10;
                    func_02012790(data_0209caa0[0x41] + 0x3c);
                    return;
                }
            }
        }
    }
    return;

sect2:
    if (data_0209caa0[0x42] != 0)
        goto clear_timer;
    {
        unsigned short *ctrlp = &data_020a0e58;
        unsigned short ctrl = *ctrlp;
        unsigned short timer;
        unsigned char r3;
        if ((ctrl & 0x30) == 0)
            goto clear_timer;
        timer = *(unsigned short *)(c + 0x106);
        if (timer != 0) {
            unsigned short *hp = (unsigned short *)(((long long)(int)(c + 0x106)) & 0xFFFFFFFFFFFFFFFFLL);
            *hp = (unsigned short)(*hp - 1);
            return;
        }
        if (*(unsigned char *)(c + 0x135) == 0)
            return;
        if (*(unsigned char *)(c + 0x133) != 1)
            return;
        if (*(unsigned char *)(c + 0x130) < 3u)
            return;
        cur = *(unsigned char *)(c + 0x134);
        r3 = cur;
        if (ctrl & 0x20) {
            if ((ctrlp[1] & 0x20) == 0 && timer != 0)
                goto join;
            *(unsigned short *)(c + 0x106) = (data_020a0e5a[idx << 1] & 0x20) ? 0x10 : 8;
            if (*(unsigned char *)(c + 0x134) != 0)
                r3 = (unsigned char)(r3 - 1);
            goto join;
        }
        if (ctrl & 0x10) {
            if ((ctrlp[1] & 0x10) == 0 && timer != 0)
                goto join;
            *(unsigned short *)(c + 0x106) = (data_020a0e5a[idx << 1] & 0x10) ? 0x10 : 8;
            if (*(unsigned char *)(c + 0x134) != *(unsigned char *)(c + 0x130) - 2)
                r3 = (unsigned char)(r3 + 1);
        }
    join:
        if (r3 == cur)
            return;
        *(unsigned char *)(c + 0x134) = r3;
        func_02012790(0x12e);
        return;
    }
clear_timer:
    *(unsigned short *)(c + 0x106) = 0;
}
