#!/usr/bin/env python3
import subprocess, pathlib, re, struct

BASE = pathlib.Path("work/ov002_020d6dac")
TIP = (BASE / "tip.c").read_text()
start = TIP.index("    {\n        u8 b2")
end = TIP.index("    _ZN11RaycastLine13SetObjAndLine")
prefix, suffix = TIP[:start], TIP[end:]

def score_src(src, name):
    p = BASE / f"{name}.c"
    p.write_text(src)
    r = subprocess.run([
        ".venv/bin/python","tools/match.py","--c",str(p),
        "--func","func_ov002_020d6dac","--addr","0x20d6dac","--size","0x284",
        "--version","1.2/sp2p3","--module","ov002"
    ], capture_output=True, text=True)
    out = r.stdout + r.stderr
    if "size differs" in out:
        m = re.search(r"candidate (0x[0-9a-f]+)", out)
        return 9000 + int(m.group(1),16) if m else 9999, out
    mism = [l for l in out.splitlines() if "MISMATCH" in l]
    matched = any("MATCHING VERSIONS:" in l and "none" not in l for l in out.splitlines())
    return (0 if matched else len(mism)), out

def wrap(b):
    return prefix + b + suffix

# Focus: materialize +0x90 (int form) + force r0 scale path after GetBodyModelID
blocks = {}

blocks["t_mtx_first"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            char *mtx = (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c;
            MulVec3Mat4x3((char *)m2i + 0x24, mtx, &p2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["t_mid_local"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            int mid = model2;
            MulVec3Mat4x3((char *)m2i + 0x24, (char *)(*(int *)(self + mid * 4 + 0xdc)) + 0x1c, &p2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["t_scale_expr"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)(m2i + 0x24), (char *)(*(int *)(self + (model2 << 2) + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["t_index_add"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)(m2i + 0x24), (char *)(*(int *)((int)self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# Use char* m2 with int add spelling that matched r3/r5 path
blocks["t_m2_char_int"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)((int)m2 + 0x90);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            char *mtx = (char *)(*(int *)(self + model2 * 4 + 0xdc));
            MulVec3Mat4x3(m2 + 0x24, mtx + 0x1c, &p2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["t_two_step_mtx"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            int *slot = (int *)(self + model2 * 4 + 0xdc);
            char *mtx = (char *)*slot + 0x1c;
            MulVec3Mat4x3((char *)m2i + 0x24, mtx, &p2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# Match block1 exactly except +0x90 on m2 via separate statement after load using pointer arithmetic on int*
blocks["t_like_b1"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((m2 + 0x90) + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# force p2 address early like block1 (r2 early)
blocks["t_dst_early"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        void *dst = &p2;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2i + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, dst);
        Vec3_MulScalarInPlace((int *)dst, 0x8000);
    }

'''

# comma operator to force order: scale matrix first
blocks["t_comma"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        char *mtx;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        mtx = (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c;
        MulVec3Mat4x3((char *)m2i + 0x24, mtx, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# try with model2 used in r0-preserving way via self[model2]
blocks["t_arr"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2i + 0x24, (char *)(((int *)(self + 0xdc))[model2]) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# change first block too? no keep first

# //cpp version of best size+4
blocks["t_cpp"] = None  # handled separately

# Try: store model into local BEFORE using m2 in mul — maybe frees r0 schedule
blocks["t_store_model"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        /* use model2 only once in a form that prefers r0 */
        MulVec3Mat4x3((void *)(m2i + 0x24), (void *)((char *)*(void **)(self + 4 * model2 + 0xdc) + 0x1c), &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# Invert: compute vec ptr into local first after call using m2i
blocks["t_vec_local"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            void *vec = (char *)m2i + 0x24;
            MulVec3Mat4x3(vec, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# key insight from block1: r2 (dst) is set BEFORE add r0,r5,#0x24
# force that order with nested calls or assignment
blocks["t_args_vars"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        {
            void *a0;
            void *a1;
            void *a2 = &p2;
            a1 = (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c;
            a0 = (char *)m2i + 0x24;
            MulVec3Mat4x3(a0, a1, a2);
        }
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# order: a1 first using model2 while in r0, then a0
blocks["t_a1_first"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        void *a1;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        a1 = (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c;
        MulVec3Mat4x3((char *)m2i + 0x24, a1, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# try using same p1-style stack - maybe p2 vs p1 offset causes order
# put p2 where p1 is? can't easily

# What if second block uses &p1's sibling ordering by using a temp Vector3 on stack with decl order change
# Change decl order of locals at top of function

def full_with_decls(block2, decl_variant):
    # rebuild function with different local decl order
    head = '''typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

struct Vector3 { int x, y, z; };
typedef struct P2 { int a, b; } P2;
typedef struct ClsnResult { void *vtb; int s0, s1, s2, s3, s4; u16 f, g; int h, i, j; } ClsnResult;
typedef struct Vec3_16 { s16 x, y, z; } Vec3_16;

extern void _ZN11RaycastLineC1Ev(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern int _ZNK6Player14GetBodyModelIDEjb(void *thiz, unsigned int a, int b);
extern void MulVec3Mat4x3(void *v, void *m, void *dst);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, void *a, void *b, void *act);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLine10GetClsnPosEv(void *res, void *self);
extern unsigned int _ZNK10ClsnResult9GetClsnIDEv(void *r);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void _ZN10ClsnResultD1Ev(void *r);
extern int func_02053274(int *a, int *b);
extern void _ZNK11SurfaceInfo12CopyNormalToER7Vector3(void *thiz, void *out);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void *_ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    unsigned int a, unsigned int b, int x, int y, int z, const void *v, void *cb);

extern short data_02082214[];
extern void *data_02099368[];

int func_ov002_020d6dac(char *self)
{
'''
    decls = {
        "orig": '''    Vec3_16 dir;
    struct Vector3 p1;
    struct Vector3 p2;
    ClsnResult res;
    struct Vector3 clsnPos;
    struct Vector3 normal;
    char rl[0x78];
    int *d;
    char *actor;
    s16 ang;
    int t;
    int idx;
''',
        "p2_first": '''    struct Vector3 p2;
    struct Vector3 p1;
    Vec3_16 dir;
    ClsnResult res;
    struct Vector3 clsnPos;
    struct Vector3 normal;
    char rl[0x78];
    int *d;
    char *actor;
    s16 ang;
    int t;
    int idx;
''',
        "rl_early": '''    char rl[0x78];
    struct Vector3 p1;
    struct Vector3 p2;
    Vec3_16 dir;
    ClsnResult res;
    struct Vector3 clsnPos;
    struct Vector3 normal;
    int *d;
    char *actor;
    s16 ang;
    int t;
    int idx;
''',
    }
    mid = '''
    _ZN11RaycastLineC1Ev(rl);

    {
        u8 b = *(u8 *)(self + 0x714);
        int model;
        char *m1 = (char *)(*(int *)(*(int *)(self + ((b << 2) + 3) * 4 + 0x154) + 0x14));
        model = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m1 + 0x24, (char *)(*(int *)(self + model * 4 + 0xdc)) + 0x1c, &p1);
        Vec3_MulScalarInPlace((int *)&p1, 0x8000);
    }

'''
    return head + decls[decl_variant] + mid + block2 + suffix

base_blk = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2i + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

results = []
for name, blk in blocks.items():
    if blk is None:
        continue
    div, out = score_src(wrap(blk), name)
    print(f"{name:20s} div={div}")
    results.append((div, name))
    if div == 0:
        print("*** MATCH ***")
        (BASE/"MATCH.c").write_text(wrap(blk))
        break
    if div < 7:
        for l in out.splitlines():
            if "MISMATCH" in l:
                print(" ", l)

for dv in ("p2_first", "rl_early"):
    src = full_with_decls(base_blk, dv)
    div, out = score_src(src, f"decl_{dv}")
    print(f"decl_{dv:16s} div={div}")
    results.append((div, f"decl_{dv}"))
    if div == 0:
        print("*** MATCH ***")
        (BASE/"MATCH.c").write_text(src)
        break
    if div < 7:
        for l in out.splitlines():
            if "MISMATCH" in l:
                print(" ", l)

# //cpp with int form
cpp_src = "//cpp\n" + wrap(base_blk)
div, out = score_src(cpp_src, "t_cpp")
print(f"{'t_cpp':20s} div={div}")
results.append((div, "t_cpp"))
if div == 0:
    (BASE/"MATCH.c").write_text(cpp_src)
    print("*** MATCH ***")

results.sort()
print("BEST", results[:12])
