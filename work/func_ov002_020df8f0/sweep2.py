#!/usr/bin/env python3
import subprocess, pathlib, textwrap, re

REPO = pathlib.Path(__file__).resolve().parents[2]
PY = REPO / ".venv/bin/python3"
TIP = pathlib.Path(__file__).with_name("tip.c").read_text()
MARKER = "    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);"
head = TIP[:TIP.index(MARKER)]

def score(path):
    r = subprocess.run(
        [str(PY), "tools/fdiff.py", "--c", str(path), "--name", "func_ov002_020df8f0",
         "--module", "ov002", "--addr", "0x20df8f0", "--size", "0x500", "--quiet"],
        cwd=REPO, capture_output=True, text=True)
    out = r.stdout + r.stderr
    m = re.search(r"mismatches=(\d+)", out)
    return int(m.group(1)) if m else -1, out.strip()

def mk_tail(**kw):
    # template pieces
    copies = kw.get("copies", """\
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
""")
    loads = kw.get("loads", """\
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
""")
    tclamp = kw.get("tclamp", """\
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
""")
    mul = kw.get("mul", """\
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
""")
    neg = kw.get("neg", """\
        s16 r = (s16)(0 - ang);
""")
    clamp = kw.get("clamp", """\
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
""")
    store = kw.get("store", """\
        *(s16*)(c + 0x8c) = r;
""")
    pre = kw.get("pre", "")
    return "    {\n" + pre + copies + loads + tclamp + mul + neg + clamp + store + "    }\n}\n"

variants = {
"base": {},
"cmp_swap": {"clamp": "        if (P < r) r = P;\n        if (-P > r) r = (s16)(-P);\n"},
"cmp_swap2": {"clamp": "        if (P < r) r = P;\n        if (r < (s16)(0-P)) r = (s16)(0-P);\n"},
"cmp_le": {"clamp": "        if (!(r <= P)) r = P;\n        if (!(r >= -P)) r = (s16)(-P);\n"},
"min_max_macro": {"clamp": "        r = r > P ? P : r;\n        r = r < (s16)(-P) ? (s16)(-P) : r;\n"},
"neg_unary": {"neg": "        s16 r = (s16)(-ang);\n"},
"neg_rsb": {"neg": "        s16 r = (s16)(0 + (0 - ang));\n"},
"P_first": {
    "mul": "        s16 P = (s16)((int)(((s64)t * 0x2aaaU + 0x800) >> 12));\n",
},
"prod_s64": {
    "mul": "        s64 wide = (s64)t * 0x2aaa + 0x800;\n        s16 P = (s16)(int)(wide >> 12);\n",
},
"u64_mul": {
    "mul": "        u64 w = (u64)(u32)t * 0x2aaaULL + 0x800;\n        s16 P = (s16)(int)(w >> 12);\n",
},
"t_gt": {"tclamp": "        int t = spd >> 5;\n        if (t > 0xfff) t = 0x1000;\n"},
"t_tern": {"tclamp": "        int t = spd >> 5;\n        t = (t >= 0x1000) ? 0x1000 : t;\n"},
"loads_ang_first": {"loads": "        s16 ang = *(s16*)(c + 0x92);\n        int spd = *(int*)(c + 0x640);\n"},
"vol_ang": {"loads": "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(volatile s16*)(c + 0x92);\n"},
"inline_no_P": {
    "mul": "",
    "neg": "        s16 r = (s16)(0 - ang);\n",
    "clamp": """\
        {
            s16 P = (s16)((int)(((s64)t * 0x2aaaU + 0x800) >> 12));
            if (r > P) r = P;
            if (r < -P) r = (s16)(-P);
        }
""",
},
"inline_mul_clamp": {
    "mul": "",
    "neg": "        s16 r = (s16)(0 - ang);\n",
    "clamp": """\
        if (r > (s16)((int)(((s64)t * 0x2aaaU + 0x800) >> 12)))
            r = (s16)((int)(((s64)t * 0x2aaaU + 0x800) >> 12));
        {
            s16 nP = (s16)(-(s16)((int)(((s64)t * 0x2aaaU + 0x800) >> 12)));
            if (r < nP) r = nP;
        }
""",
},
"hi0_decl_first": {
    "pre": "        int hi = 0;\n",
    "mul": "        int prod = (int)(((s64)t * ((u64)0x2aaa | ((u64)hi << 32)) + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"const_local": {
    "pre": "        const int K = 0x2aaa;\n",
    "mul": "        int prod = (int)(((s64)t * (unsigned)K + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"comma_hi": {
    "copies": """\
        s16 a0 = *(s16*)(c + 0x92);
        int hi;
        *(s16*)(c + 0x8c) = (hi = 0, a0);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
""",
    "mul": "        int prod = (int)(((s64)t * (0x2aaaLL + ((s64)hi << 32)) + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"r_reuse_a0": {
    "copies": """\
        s16 r = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8c) = r;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
""",
    "loads": "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
    "neg": "        r = (s16)(0 - ang);\n",
},
"P_then_r_cmp": {
    "clamp": "        if (P < r) r = P;\n        { s16 n = (s16)(0 - P); if (n > r) r = n; }\n",
},
"equal_arm": {
    "clamp": "        r = (r > P) ? P : ((r < -P) ? (s16)(-P) : r);\n",
},
"nested_if": {
    "clamp": "        if (r > P) { r = P; } else if (r < -P) { r = (s16)(-P); }\n",
},
"store_direct": {
    "store": "        *(s16*)(c + 0x8c) = (s16)r;\n",
},
"t_assign_mul": {
    "tclamp": "        int t = spd >> 5;\n        if (t >= 0x1000) t = 0x1000;\n",
    "mul": "        t = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);\n        s16 P = (s16)t;\n",
},
"fix12_style": {
    "mul": "        int prod = (int)(((s64)t * 0x2aaa + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"mul_order": {
    "mul": "        int prod = (int)(((s64)0x2aaaU * t + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"add_before_shift": {
    "mul": "        s64 w = (s64)t * 0x2aaaU;\n        w = w + 0x800;\n        s16 P = (s16)(int)(w >> 12);\n",
},
"cmp_swap_Pfirst": {
    "clamp": "        if (P < r) r = P;\n        if (-P > r) r = (s16)(-P);\n",
    "mul": "        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);\n",
},
"cmp_swap_loads_ang": {
    "clamp": "        if (P < r) r = P;\n        if (-P > r) r = (s16)(-P);\n",
    "loads": "        s16 ang = *(s16*)(c + 0x92);\n        int spd = *(int*)(c + 0x640);\n",
},
"dummy_r2": {
    "pre": "        register int z asm(\"r2\");\n        z = 0;\n",
    "mul": "        int prod = (int)(((s64)t * (0x2aaaLL | ((s64)z << 32)) + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"volatile_hi": {
    "pre": "        volatile int hi = 0;\n",
    "mul": "        int prod = (int)(((s64)t * (0x2aaaLL | ((s64)hi << 32)) + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"two_zero": {
    "pre": "        int z0 = 0;\n        int z1 = 0;\n",
    "mul": "        int prod = (int)(((s64)t * (0x2aaaLL | ((s64)(z0|z1) << 32)) + 0x800) >> 12);\n        s16 P = (s16)prod;\n",
},
"ang_in_r_tmp": {
    "loads": "        int spd = *(int*)(c + 0x640);\n        int ang32 = *(s16*)(c + 0x92);\n",
    "neg": "        s16 r = (s16)(0 - ang32);\n",
},
"spd_u32": {
    "loads": "        u32 spd = *(u32*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
},
"cast_char_loads": {
    "loads": "        int spd = *(int*)((char*)c + 0x640);\n        s16 ang = *(s16*)((char*)c + 0x92);\n",
},
"launder_loads": {
    "loads": "        int spd = *(int*)(((s64)(int)(c + 0x640)) & 0xFFFFFFFFFFFFFFFFLL);\n        s16 ang = *(s16*)(c + 0x92);\n",
},
}

outdir = pathlib.Path(__file__).parent / "sweep2"
outdir.mkdir(exist_ok=True)
results = []
for name, kw in variants.items():
    src = head + mk_tail(**kw)
    p = outdir / f"{name}.c"
    p.write_text(src)
    div, line = score(p)
    results.append((div if div >= 0 else 9999, name, line))
    print(f"{div:4d}  {name}")

results.sort()
print("\n=== BEST ===")
for div, name, line in results[:20]:
    print(f"{div:4d}  {name}  {line[:80]}")
