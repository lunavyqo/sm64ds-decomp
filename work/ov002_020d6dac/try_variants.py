#!/usr/bin/env python3
"""Try C variants for second bone block to force ROM schedule."""
import subprocess, re, pathlib, textwrap, json

BASE = pathlib.Path("work/ov002_020d6dac")
TIP = (BASE / "tip.c").read_text()

# extract prefix/suffix around second block
start = TIP.index("    {\n        u8 b2")
end = TIP.index("    _ZN11RaycastLine13SetObjAndLine")
prefix = TIP[:start]
suffix = TIP[end:]

def wrap(block2):
    return prefix + block2 + suffix

def score(src, name):
    p = BASE / f"{name}.c"
    p.write_text(src)
    r = subprocess.run([
        ".venv/bin/python","tools/match.py",
        "--c", str(p),
        "--func","func_ov002_020d6dac","--addr","0x20d6dac","--size","0x284",
        "--version","1.2/sp2p3","--module","ov002"
    ], capture_output=True, text=True)
    out = r.stdout + r.stderr
    mism = len([l for l in out.splitlines() if "MISMATCH" in l])
    matched = "MATCHING VERSIONS: none" not in out and "MATCH" in out
    # better: check for exact phrase
    ok = "MATCHING VERSIONS: 1.2/sp2p3" in out or re.search(r"MATCHING VERSIONS:.*1\.2/sp2p3", out)
    if "size differs" in out or "error" in out.lower() and mism==0 and not ok:
        # compile fail?
        if "Error" in out or "error:" in out or "failed" in out.lower():
            return 999, out[-1500:]
    return mism, out if mism < 7 or ok else ""

VARIANTS = {}

VARIANTS["v_launder_u64"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)(((long long)(int)m2 + 0x90) & 0xffffffffLL);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_launder_u64_mask"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)((unsigned long long)(unsigned)m2 + 0x90);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_two_var"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *base2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        char *m2 = base2 + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_vol"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        *(char * volatile *)&m2 = m2 + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_vol_ptr"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char * volatile m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = m2 + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_int_add"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14);
        m2i += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2i + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_int_add_launder"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14);
        m2i = (int)((long long)m2i + 0x90);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)(m2i + 0x24), (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_split_node"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int *node = *(int **)(self + ((b2 << 2) + 3) * 4 + 0x154);
        char *m2 = (char *)node[5]; /* 0x14/4 = 5 */
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_b_plus_offset"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14)) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_mul_b4_direct"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0xb4, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_force_use"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        char *m2p;
        m2p = m2 + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, (int)(m2p == m2p));
        /* force m2p live: third arg still 0 if equal */
        MulVec3Mat4x3(m2p + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_comma"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = (m2 += 0x90, _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0));
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_reorder_model_first"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

VARIANTS["v_cast_int"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        unsigned m2u = (unsigned)*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14);
        m2u += 0x90u;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((void *)(m2u + 0x24u), (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# also try pragma around second block
VARIANTS["v_pragma_nosched"] = '''#pragma dont_inline on
    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }
#pragma dont_inline off

'''

results = []
for name, blk in VARIANTS.items():
    src = wrap(blk)
    mism, extra = score(src, name)
    results.append((mism, name))
    print(f"{name:30s} div={mism}")
    if mism == 0:
        print("*** MATCH ***", name)
        (BASE / "MATCH.c").write_text(src)
        break
    if mism < 7:
        print(f"  IMPROVED to {mism}")
        # show mismatches
        r = subprocess.run([
            ".venv/bin/python","tools/match.py",
            "--c", str(BASE / f"{name}.c"),
            "--func","func_ov002_020d6dac","--addr","0x20d6dac","--size","0x284",
            "--version","1.2/sp2p3","--module","ov002"
        ], capture_output=True, text=True)
        for l in r.stdout.splitlines():
            if "MISMATCH" in l:
                print(" ", l)

results.sort()
print("BEST:", results[:5])
