/* storage 0x023A11E0 runtime 0x027D3158 linked 0x14C main ARM. */

extern void fn_023969D8(void *dst, void *src);
extern void fn_023A0CE0(unsigned short *node, unsigned short *buf);
extern void fn_023A0F58(void *node, void *buf);
extern void fn_02394DF8(void *list, int node);
extern void fn_0239477C(int a, int b);

void fn_023A11E0(void) {
    /* Both receive paths use seven fields; 023A0F58 copies all 16 bytes.
     * The final halfword remains untouched, as in the retail caller. */
    unsigned short buf[8];
    int *base;
    unsigned short *rec;
    unsigned short *p;
    unsigned int flags;
    unsigned int bit;
    unsigned int nib;

    base = *(int **)0x0380FFF4;
    rec = *(unsigned short **)((char *)base + 0x1AC);
    if (rec == (unsigned short *)0xFFFFFFFF)
        return;
    p = (unsigned short *)((char *)rec + 0x10);
    if (*(unsigned short *)((char *)base + 0x310) == 0x40 && p[8] <= 0x5FC) {
        flags = p[10];
        bit = (unsigned int)((int)flags << 23) >> 31;
        if (bit != 0) {
            fn_023969D8(buf, (char *)p + 0x24);
            flags = p[10];
            bit = (unsigned int)((int)flags << 22) >> 31;
            if (bit != 0)
                goto cleanup;
            fn_023969D8(buf + 3, (char *)p + 0x1E);
        } else {
            fn_023969D8(buf, (char *)p + 0x18);
            flags = p[10];
            bit = (unsigned int)((int)flags << 22) >> 31;
            if (bit != 0)
                fn_023969D8(buf + 3, (char *)p + 0x24);
            else
                fn_023969D8(buf + 3, (char *)p + 0x1E);
        }
        buf[6] = p[21];
        flags = p[10];
        bit = (unsigned int)((int)flags << 21) >> 31;
        if (bit != 1)
            goto use_ce0;
        nib = (unsigned int)((int)p[21] << 28) >> 28;
        if (nib != 0)
            goto use_ce0;
        fn_023A0F58((char *)p + 8, buf);
        goto cleanup;
    use_ce0:
        fn_023A0CE0(p + 4, buf);
    }
cleanup:
    fn_02394DF8((char *)*(int **)0x0380FFF4 + 0x1AC, (int)rec);
    if (*(unsigned short *)((char *)*(int **)0x0380FFF4 + 0x1B4) != 0)
        fn_0239477C(2, 9);
}
