struct P {
    int a;
    int b;
};

struct Entry {
    char *f0;
    int f4;
    unsigned short f8;
    unsigned short fa;
    int fc;
    int f10;
};

extern void FS_InitFile(int *s);
extern void func_02018598(struct P *c, unsigned int arg);
extern int func_02018dc4(int a, struct P v);
extern int LoadArchive(int idx);
extern void Crash(void);
extern int func_0204eda4(int r0, char *a, unsigned int idx);

extern struct Entry data_0208ecf4[];
extern unsigned char data_0208eb54[];

int func_020185c0(int a0, unsigned int a1)
{
    int result = 0;

    FS_InitFile((int *)a0);

    if (a1 < 0x8000) {
        struct P arg;
        struct P v;
        func_02018598(&v, a1);
        arg.a = v.a;
        arg.b = v.b;
        result = func_02018dc4(a0, arg);
    } else {
        unsigned int i = 0;
        struct Entry *e = data_0208ecf4;
        do {
            if (e->f8 <= a1 && a1 < e->fa) {
                if (e->f0 == 0) {
                    if (LoadArchive(i) != 0)
                        data_0208eb54[0] = (unsigned char)i;
                    if (e->f0 == 0) {
                        Crash();
                        return 0;
                    }
                }
                result = func_0204eda4(a0, e->f0, a1 - e->f8);
                break;
            }
            i++;
            e++;
        } while (i < 0xd);
    }

    if (result == 0)
        Crash();
    return result;
}

struct S82 {
    void *f0; /* +0 */
    char pad4[4];
    unsigned short f8; /* +8 */
    unsigned short fa; /* +0xa */
    char padc[8];
};
extern struct S82 data_0208ecf4[];
extern unsigned char data_0208eb54;
extern int LoadArchive(int idx);
extern void Crash(void);
extern int func_0204ede8(void *o, unsigned int i);

int func_020186c0(unsigned int val) {
    unsigned int i;
    struct S82 *e;
    if (val < 0x8000)
        return 0;
    e = data_0208ecf4;
    for (i = 0; i < 0xd; i++) {
        if (e->f8 <= val && val < e->fa) {
            if (e->f0 == 0) {
                if (LoadArchive(i))
                    data_0208eb54 = (unsigned char)i;
                if (e->f0 == 0) {
                    Crash();
                    return 0;
                }
            }
            return func_0204ede8(e->f0, val - e->f8);
        }
        e++;
    }
    Crash();
    return 0;
}

/* func_02018770 at 0x02018770
 * Unloads the archive a file read mounted for itself, if there is one
 * (data_0208eb54 holds its index), then sets the index to -1 (0xff, none).
 */

typedef signed char s8;

extern s8 data_0208eb54; /* archive a read mounted, -1 when none */
extern void UnloadArchive(int i);

void func_02018770(void) {
    if (data_0208eb54 < 0) {
        return;
    }
    UnloadArchive(data_0208eb54);
    data_0208eb54 = -1;
}
