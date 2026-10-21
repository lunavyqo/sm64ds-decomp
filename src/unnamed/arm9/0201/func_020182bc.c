struct V2 { int x, y; };
extern int func_02018e3c(void *out_file_id, const char *path);
extern int func_020182ec(struct V2 file, int flag, ...);

void *func_020182bc(const char *path)
{
    struct V2 file;
    func_02018e3c(&file, path);
    return (void *)func_020182ec(file, 1);
}

struct V2 {
    int x, y;
};

extern void FS_InitFile(int *s);
extern int func_02018dc4(int buf, struct V2 ab);
extern void Crash(void);
extern int func_0201834c(int *buf, int x);

int func_020182ec(struct V2 file, int c, ...) {
    int buf[0x11];
    int r = 0;
    FS_InitFile(buf);
    if (func_02018dc4((int)buf, file) == 0)
        Crash();
    else
        r = func_0201834c(buf, c);
    return r;
}

/* func_0201834c at 0x0201834c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
struct Obj {
    char pad[0x20];
    unsigned int cur;
    unsigned int end;
};

extern void *_ZN6Memory8AllocateEji(unsigned int size, int align);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern int func_02018d48(struct Obj *a, void *b, unsigned int c);
extern int func_02018568(void *a);
extern unsigned int func_02018ac4(void *p);
extern void *_ZN6Memory8AllocateEj(unsigned int size);
extern void DecompressLZ16(void *src, void *dst);
extern void CpuCopy8(void *a, void *b, unsigned int c);
extern void _ZN6Memory10DeallocateEPv(void *p);
extern void Crash(void);
extern void FS_CloseFile(struct Obj *a);
extern void func_02018770(void);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);

unsigned int func_0201834c(struct Obj *self, int flag) {
    unsigned int result = 0;
    unsigned int len = self->end - self->cur;
    if (len > 8) {
        void *buf = _ZN6Memory8AllocateEji(len, -4);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)buf, len);
        int r = func_02018d48(self, buf, len);
        if (r != len)
            return result;
        if (func_02018568(buf) != 0) {
            len = func_02018ac4((void *)((char *)buf + 4));
            result = (unsigned int)_ZN6Memory8AllocateEj(len);
            DecompressLZ16((void *)((char *)buf + 4), (void *)result);
        } else {
            result = (unsigned int)_ZN6Memory8AllocateEj(len);
            CpuCopy8(buf, (void *)result, len);
        }
        _ZN6Memory10DeallocateEPv(buf);
    } else {
        Crash();
    }
    FS_CloseFile(self);
    func_02018770();
    if (flag != 0)
        _ZN4CP1514FlushDataCacheEjj(result, len);
    return result;
}
