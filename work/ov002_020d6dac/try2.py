#!/usr/bin/env python3
import subprocess, pathlib, re

BASE = pathlib.Path("work/ov002_020d6dac")
TIP = (BASE / "tip.c").read_text()
start = TIP.index("    {\n        u8 b2")
end = TIP.index("    _ZN11RaycastLine13SetObjAndLine")
prefix, suffix = TIP[:start], TIP[end:]

# Also allow full-file variants
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
        m = re.search(r"size differs: target (0x[0-9a-f]+) vs candidate (0x[0-9a-f]+)", out)
        return 1000, m.group(0) if m else "size differs", out
    mism = [l for l in out.splitlines() if "MISMATCH" in l]
    ok = "MATCHING VERSIONS: none" not in out and "=== mwccarm" in out
    # check last line
    matched = any("MATCHING VERSIONS:" in l and "none" not in l for l in out.splitlines())
    return (0 if matched else len(mism)), ("MATCH" if matched else f"div={len(mism)}"), out

def wrap(block2, file_prefix=""):
    return file_prefix + prefix + block2 + suffix

blocks = {}

blocks["b_pragma_cs"] = '''#pragma opt_common_subs off
    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }
#pragma opt_common_subs on

'''

blocks["b_pragma_sr"] = '''#pragma opt_strength_reduction off
    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }
#pragma opt_strength_reduction on

'''

blocks["b_pragma_prop"] = '''#pragma opt_propagation off
    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }
#pragma opt_propagation on

'''

blocks["b_int_add"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14);
        char *m2 = (char *)(m2i + 0x90);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_int_then_ptr"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int m2i = *(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)m2i + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_struct"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        struct { char pad[0x90]; char body[4]; } *o =
            (void *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(o->body + 0x20, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        /* body is at +0x90, body+0x20 = +0xb0 — wrong */
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# body at 0x90, want +0x24 from there → field at 0xb4 from base
blocks["b_struct2"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        struct { char pad[0x90]; char at90; } *o =
            (void *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        char *m2 = &o->at90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_struct3"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        struct S { char pad[0x90]; char v[0x30]; } *o =
            (void *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(&o->v[0x24], (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# pointer to offset 0x90 as typed
blocks["b_m2_only_plus24"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14)) + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# force schedule: compute model args first into locals
blocks["b_locals_order"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        unsigned charid = *(int *)(self + 8) & 0xff;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, charid, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# Match first block pattern more closely - use same var names structure
blocks["b_same_as_b1_plus"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24 + 0x90, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_xor0"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)((int)m2 + 0x90);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_or0"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)(((int)m2 + 0x90) | 0);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_and_ff"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 = (char *)(((unsigned)m2 + 0x90u) & 0xffffffffu);
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# u64 launder only on the +0x24 use after call, with m2+=0x90 before
blocks["b_launder_use"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        char *m2 = (char *)(*(int *)(*(int *)(self + ((b2 << 2) + 3) * 4 + 0x154) + 0x14));
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3((char *)(((long long)(int)m2 + 0x24) & 0xffffffffLL),
            (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# split double deref like ROM: first load to r2 (reuse)
blocks["b_split_loads"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int *tmp = (int *)(self + ((b2 << 2) + 3) * 4 + 0x154);
        int *node = (int *)*tmp;
        char *m2 = (char *)*(int *)((char *)node + 0x14);
        m2 += 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

blocks["b_node5"] = '''    {
        u8 b2 = *(u8 *)(self + 0x714);
        int model2;
        int *node = *(int **)(self + ((b2 << 2) + 3) * 4 + 0x154);
        char *m2 = (char *)*(node + 5); /* 0x14 */
        m2 = m2 + 0x90;
        model2 = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        MulVec3Mat4x3(m2 + 0x24, (char *)(*(int *)(self + model2 * 4 + 0xdc)) + 0x1c, &p2);
        Vec3_MulScalarInPlace((int *)&p2, 0x8000);
    }

'''

# try full-file pragma at top
file_pragmas = [
    ("#pragma opt_common_subs off\n", "fp_cs"),
    ("#pragma opt_strength_reduction off\n", "fp_sr"),
    ("#pragma opt_propagation off\n", "fp_prop"),
    ("#pragma opt_global_lifetime off\n", "fp_gl"),
    ("#pragma optimization_level 3\n", "fp_o3"),
]

results = []
for name, blk in blocks.items():
    src = wrap(blk)
    div, msg, out = score_src(src, name)
    print(f"{name:25s} {msg}")
    results.append((div, name, msg))
    if div == 0:
        print("*** MATCH ***")
        (BASE/"MATCH.c").write_text(src)
        break
    if 0 < div < 7:
        for l in out.splitlines():
            if "MISMATCH" in l:
                print(" ", l)

# full file pragmas with tip block
tip_blk = TIP[start:end]
for pref, tag in file_pragmas:
    src = wrap(tip_blk, pref)
    div, msg, out = score_src(src, tag)
    print(f"{tag:25s} {msg}")
    results.append((div, tag, msg))
    if div == 0:
        print("*** MATCH ***")
        (BASE/"MATCH.c").write_text(src)
        break

results.sort(key=lambda x: x[0])
print("TOP:", results[:10])
