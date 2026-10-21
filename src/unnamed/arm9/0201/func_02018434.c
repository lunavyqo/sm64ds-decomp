struct P { int a, b; };
extern void func_02018e3c(struct P *buf, int x);
extern void func_02018474(struct P p, int b, int c);
void func_02018434(int a0, int a1, int a2){
  struct P buf;
  func_02018e3c(&buf, a0);
  func_02018474(buf, a1, a2);
}

/* func_02018474 at 0x02018474
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct V2 {
    int x, y;
};

extern void FS_InitFile(int *s);
extern int func_02018dc4(int buf, struct V2 ab);
extern void Crash(void);
extern int func_020184e0(int *buf, unsigned int c, unsigned int d);

int func_02018474(int a, int b, int c, int d, ...) {
    int buf[0x11];
    FS_InitFile(buf);
    if (func_02018dc4((int)buf, *(struct V2 *)&a) == 0) {
        Crash();
        return 0;
    }
    return func_020184e0(buf, c, d);
}

struct Obj {
    char pad[0x20];
    unsigned int cur;
    unsigned int end;
};

extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_02018d48(void *a, unsigned int b, unsigned int c);
extern void FS_CloseFile(void *a);
extern void func_02018770(void);
extern int func_02018568(unsigned int a);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

int func_020184e0(struct Obj *self, unsigned int buf, unsigned int n) {
    int r;
    unsigned int len = self->end - self->cur;
    if (len > n)
        len = n;
    _ZN4CP1527FlushAndInvalidateDataCacheEjj(buf, len);
    r = func_02018d48(self, buf, len);
    FS_CloseFile(self);
    func_02018770();
    if (len != r)
        return 0;
    func_02018568(buf);
    _ZN4CP1514FlushDataCacheEjj(buf, len);
    return len;
}
