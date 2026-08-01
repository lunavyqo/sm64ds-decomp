#!/usr/bin/env python3
import subprocess, pathlib, re, textwrap

REPO = pathlib.Path(__file__).resolve().parents[2]
PY = REPO / ".venv/bin/python3"
TIP = pathlib.Path(__file__).with_name("tip.c").read_text()
MARKER = "    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);"
head = TIP[:TIP.index(MARKER)]

def score(src, name):
    p = pathlib.Path(__file__).parent / "sweep3" / f"{name}.c"
    p.parent.mkdir(exist_ok=True)
    p.write_text(src)
    r = subprocess.run(
        [str(PY), "tools/fdiff.py", "--c", str(p), "--name", "func_ov002_020df8f0",
         "--module", "ov002", "--addr", "0x20df8f0", "--size", "0x500", "--quiet"],
        cwd=REPO, capture_output=True, text=True)
    out = (r.stdout + r.stderr).strip()
    m = re.search(r"mismatches=(\d+)", out)
    div = int(m.group(1)) if m else 9999
    print(f"{div:4d}  {name}")
    return div, p, out

# base head with cmp_swap tail as starting point
def tail_cmp_swap(extra_pre="", loads=None, mul=None, more_pragmas=""):
    loads = loads or """\
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
"""
    mul = mul or """\
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
"""
    return f"""    {{
{extra_pre}        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
{loads}        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
{mul}        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }}
}}
"""

results = []

# 1. pragma combos on tip
for prag in [
    "#pragma opt_common_subs off\n",
    "#pragma opt_common_subs off\n#pragma opt_strength_reduction off\n",
    "#pragma opt_common_subs off\n#pragma opt_propagation off\n",
    "#pragma opt_common_subs off\n#pragma opt_strength_reduction off\n#pragma opt_propagation off\n",
    "#pragma opt_common_subs off\n#pragma opt_global_lifetime off\n",
    "#pragma opt_common_subs off\n#pragma opt_dead_assignments off\n",
    "#pragma opt_common_subs off\n#pragma scheduling off\n",
    "#pragma opt_common_subs off\n#pragma opt_vectorize off\n",
]:
    h = TIP[:TIP.index("#pragma")] + prag + TIP[TIP.index("void"):TIP.index(MARKER)]
    name = "prag_" + prag.replace("#pragma ","").replace("\n","_").replace(" ","")[:40]
    d,_,_ = score(h + tail_cmp_swap(), name)
    results.append((d, name))

# 2. force r1 live during zero via early const load that prefers r1
for name, pre, loads, mul in [
    ("early_k", "        int K = 0x2aaa;\n", 
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
     "        int prod = (int)(((s64)t * (unsigned)K + 0x800) >> 12);\n        s16 P = (s16)prod;\n"),
    ("early_k_u", "        unsigned K = 0x2aaa;\n",
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
     "        int prod = (int)(((s64)t * K + 0x800) >> 12);\n        s16 P = (s16)prod;\n"),
    ("k_after_copy", "",
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n        int K = 0x2aaa;\n",
     "        int prod = (int)(((s64)t * (unsigned)K + 0x800) >> 12);\n        s16 P = (s16)prod;\n"),
    # Use Fix12-style multiply via smull of t and constant from pool - force order
    ("mul_cast_u32_t", "",
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
     "        int prod = (int)(((s64)(u32)t * 0x2aaaU + 0x800) >> 12);\n        s16 P = (s16)prod;\n"),
    # signed * signed with explicit casts
    ("mul_ss", "",
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
     "        int prod = (int)(((s64)(s32)t * (s64)(s32)0x2aaa + 0x800) >> 12);\n        s16 P = (s16)prod;\n"),
    # Interleave: compute P before neg differently  
    ("P_r_order", "",
     "        int spd = *(int*)(c + 0x640);\n        s16 ang = *(s16*)(c + 0x92);\n",
     "        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);\n"),
    # Use int for r and P then cast at store
    ("int_r_P", "",
     "        int spd = *(int*)(c + 0x640);\n        int ang = *(s16*)(c + 0x92);\n",
     "        int P = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);\n        P = (s16)P;\n"),
]:
    # special tail for int_r_P
    if name == "int_r_P":
        t = f"""    {{
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        int ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        int r = 0 - ang;
        r = (s16)r;
        if (P < r) r = P;
        if (-P > r) r = -P;
        r = (s16)r;
        *(s16*)(c + 0x8c) = (s16)r;
    }}
}}
"""
        d,_,_ = score(head + t, name)
    else:
        d,_,_ = score(head + tail_cmp_swap(extra_pre=pre, loads=loads, mul=mul), name)
    results.append((d, name))

# 3. pressure r1: keep a dummy value in a local that might take r1
for name, body in [
    ("live_one", """\
    {
        int one = 1; (void)one;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    ("sink_zero_use", """\
    {
        int hi = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640) + hi;
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    # Structure like ROM: copies then compute using (s64) extension of const via union
    ("union_k", """\
    {
        union { s64 wide; struct { u32 lo; u32 hi; } w; } k;
        k.w.lo = 0x2aaa;
        k.w.hi = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * k.wide + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    # Array escape for coloring
    ("arr_k", """\
    {
        u32 kv[2] = {0x2aaa, 0};
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * ((s64)kv[0] | ((s64)kv[1] << 32)) + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    # Statement expr GNU
    ("stmt_expr", """\
    {
        s16 r;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        {
            int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
            s16 P = (s16)prod;
            r = (s16)(0 - ang);
            if (P < r) r = P;
            if (-P > r) r = (s16)(-P);
        }
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    # Reorder: mul first then copies? (probably breaks schedule)
    ("mul_before_copies", """\
    {
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
    # Use original tip clamp (r > P) with int types
    ("orig_clamp_int", """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""),
]:
    d,_,_ = score(head + body, name)
    results.append((d, name))

# 4. //cpp version
cpp_head = "//cpp\n" + head
d,_,_ = score(cpp_head + tail_cmp_swap(), "cpp_cmp_swap")
results.append((d, "cpp_cmp_swap"))

# 5. Try removing launders from head (maybe they affect coloring?)
# Count launders in tip
print("launder count", TIP.count("0xFFFFFFFFFFFFFFFF"))

results.sort()
print("\n=== BEST ===")
for d,n in results[:15]:
    print(f"{d:4d}  {n}")
