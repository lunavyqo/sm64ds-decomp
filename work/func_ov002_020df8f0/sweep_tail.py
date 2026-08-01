#!/usr/bin/env python3
"""Sweep final-block spellings for func_ov002_020df8f0; score with fdiff --quiet."""
import subprocess, pathlib, textwrap, re, sys

REPO = pathlib.Path(__file__).resolve().parents[2]
PY = REPO / ".venv/bin/python3"
TIP = pathlib.Path(__file__).with_name("tip.c").read_text()

# Split tip into head (everything before final field-copy + clamp) and keep head fixed.
# Head ends at the a8/98 stores and before 0x8c copies.
MARKER = "    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);"
assert MARKER in TIP
head = TIP[:TIP.index(MARKER)]

tails = {}

# A: baseline tip tail
tails["baseline"] = TIP[TIP.index(MARKER):]

# B: no pragma (remove from head)
# C: various final blocks

tails["ang_first_decl"] = textwrap.dedent("""\
    {
        s16 ang = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8c) = ang;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["hi_early"] = textwrap.dedent("""\
    {
        int hi = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * ((s64)0x2aaa | ((s64)hi << 32)) + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["spd_before_ang"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        u32 kh = 0;
        int prod = (int)((((s64)(u32)t | ((s64)kh << 32)) * 0x2aaa + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["u64_k"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        u64 k = 0x2aaaULL;
        int prod = (int)(((s64)t * (s64)k + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["interleave_zero"] = textwrap.dedent("""\
    {
        s16 a0 = *(s16*)(c + 0x92);
        int hi = 0;
        *(s16*)(c + 0x8c) = a0;
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * (0x2aaa + ((s64)hi << 32)) + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["cmp_swap"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(-ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["c89_top"] = textwrap.dedent("""\
    {
        int spd;
        s16 ang;
        int t;
        int prod;
        s16 P;
        s16 r;
        int hi;
        hi = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        spd = *(int*)(c + 0x640);
        ang = *(s16*)(c + 0x92);
        t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        P = (s16)prod;
        r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["mul_s64_plain"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * (s64)0x2aaa + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["ang_spd_order"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        s16 ang = *(s16*)(c + 0x92);
        int spd = *(int*)(c + 0x640);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["ternary_clamp"] = textwrap.dedent("""\
    {
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
        r = (r > P) ? P : r;
        r = (r < -P) ? (s16)(-P) : r;
        *(s16*)(c + 0x8c) = r;
    }
}
""")

tails["vol_spd"] = textwrap.dedent("""\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(volatile int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""")

# no-pragma variants of best
def strip_pragma(h):
    return h.replace("#pragma opt_common_subs off\n", "")

def score(path):
    r = subprocess.run(
        [str(PY), "tools/fdiff.py", "--c", str(path), "--name", "func_ov002_020df8f0",
         "--module", "ov002", "--addr", "0x20df8f0", "--size", "0x500", "--quiet"],
        cwd=REPO, capture_output=True, text=True)
    m = re.search(r"mismatches=(\d+)", r.stdout + r.stderr)
    return int(m.group(1)) if m else -1, (r.stdout + r.stderr).strip()

outdir = pathlib.Path(__file__).parent / "sweep"
outdir.mkdir(exist_ok=True)

results = []
for name, tail in tails.items():
    for tag, h in [("pragma", head), ("noprag", strip_pragma(head))]:
        src = h + tail
        # if tail is full tip baseline, handle specially
        if name == "baseline" and tag == "pragma":
            src = TIP
        elif name == "baseline" and tag == "noprag":
            src = strip_pragma(TIP)
        p = outdir / f"{name}_{tag}.c"
        p.write_text(src)
        div, line = score(p)
        results.append((div, f"{name}_{tag}", line))
        print(f"{div:4d}  {name}_{tag}  {line}")

results.sort()
print("\n=== BEST ===")
for div, name, line in results[:15]:
    print(f"{div:4d}  {name}")
