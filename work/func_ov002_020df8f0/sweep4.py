#!/usr/bin/env python3
import subprocess, pathlib, re

REPO = pathlib.Path(__file__).resolve().parents[2]
PY = REPO / ".venv/bin/python3"
TIP = pathlib.Path(__file__).with_name("tip.c").read_text()
MARKER = "    *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);"
head = TIP[:TIP.index(MARKER)]

def score(src, name):
    p = pathlib.Path(__file__).parent / "sweep4" / f"{name}.c"
    p.parent.mkdir(exist_ok=True)
    p.write_text(src)
    r = subprocess.run(
        [str(PY), "tools/fdiff.py", "--c", str(p), "--name", "func_ov002_020df8f0",
         "--module", "ov002", "--addr", "0x20df8f0", "--size", "0x500", "--quiet"],
        cwd=REPO, capture_output=True, text=True)
    out = (r.stdout + r.stderr).strip()
    m = re.search(r"mismatches=(\d+)", out)
    div = int(m.group(1)) if m else 9999
    # also detect mov r2,#0 at mismatch start
    r2 = subprocess.run(
        [str(PY), "tools/fdiff.py", "--c", str(p), "--name", "func_ov002_020df8f0",
         "--module", "ov002", "--addr", "0x20df8f0", "--size", "0x500",
         "--candidate-start", "0x44c", "--candidate-length", "0x10"],
        cwd=REPO, capture_output=True, text=True)
    has_r2 = "mov r2, #0" in r2.stdout and "mov r1, #0" not in r2.stdout.split("\n")[0] if r2.stdout else False
    # simpler: look at cand side
    for line in r2.stdout.splitlines():
        if "0x44c" in line or "+0x44c" in line:
            flag = "R2" if "mov r2" in line.split("|")[-2] if "|" in line else ("?" )
            # candidate is last column before mismatch
            parts = [x.strip() for x in line.split("|")]
            if len(parts) >= 3:
                cand = parts[2]
                flag = "r2" if "r2" in cand else ("r1" if "r1" in cand else cand[:20])
            print(f"{div:4d}  {name:30s}  zero_reg={flag}")
            return div
    print(f"{div:4d}  {name}")
    return div

# Function-scope decls before the final block (C89 style at function level)
# Insert before final marker by closing previous and adding decls - actually inject at start of function body

def inject_func_locals(locals_decl, tail_body):
    # after void func(...) {\n
    idx = head.index("{\n") + 2
    h = head[:idx] + locals_decl + head[idx:]
    return h + tail_body

tail_std = """\
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
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
"""

# Use function-level temps assigned in tail
for decl, assign_tail, name in [
    ("    int spd; s16 ang; int t; s16 P; s16 r;\n",
     """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        spd = *(int*)(c + 0x640);
        ang = *(s16*)(c + 0x92);
        t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "fndecl_std"),
    ("    int spd; s16 ang; int t; s16 P; s16 r; int hi;\n",
     """\
    {
        hi = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        spd = *(int*)(c + 0x640);
        ang = *(s16*)(c + 0x92);
        t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        P = (s16)(int)(((s64)t * (0x2aaaLL | ((s64)hi << 32)) + 0x800) >> 12);
        r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "fndecl_hi"),
    ("    s16 r; s16 P; int t; int spd; s16 ang;\n",  # reverse rank order
     """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        spd = *(int*)(c + 0x640);
        ang = *(s16*)(c + 0x92);
        t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "fndecl_rev"),
]:
    score(inject_func_locals(decl, assign_tail), name)

# Try: don't use named ang - reload for neg
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int prod = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 P = (s16)prod;
        s16 r = (s16)(0 - *(s16*)(c + 0x92));
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "reload_ang")

# ang loaded into r, then r = -r after mul
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 r = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        r = (s16)(0 - r);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "r_is_ang")

# Force spd into a pointer temp first  
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int *ps = (int*)(c + 0x640);
        s16 *pa = (s16*)(c + 0x92);
        int spd = *ps;
        s16 ang = *pa;
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "ptr_temps")

# Interleave mul setup with -ang: compute -ang first into r, keep through mul
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        s16 r = (s16)(0 - *(s16*)(c + 0x92));
        int spd = *(int*)(c + 0x640);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "neg_first")

# Match ROM interleave: -ang happens during the 64-bit add
# Write as: P = ..., r = -ang, clamp  with r computed as expression that might schedule mid-mul
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s64 w = (s64)t * 0x2aaaU + 0x800;
        s16 r = (s16)(0 - ang);
        s16 P = (s16)(int)(w >> 12);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "wide_then_neg")

# Use smull-style via Fix12: (t * 0x2aaa + 0x800) >> 12 as two-step
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s64 w = (s64)t * 0x2aaa;
        w += 0x800;
        s16 P = (s16)(int)(w >> 12);
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "wide_add_sep")

# Critical: maybe copies should leave ang in r0 from first load for reuse as ang
# i.e. first store uses ang that is reloaded for mul - but ROM reloads.
# What if we DON'T copy 0x92->0x8c as separate, but the final store overwrites?
# ROM does both copy and final store.

# Try halfword pointer walk
score(head + """\
    {
        s16 *d = (s16*)(c + 0x8c);
        s16 *s = (s16*)(c + 0x92);
        d[0] = s[0];
        d[1] = s[1];
        d[2] = s[2];
        int spd = *(int*)(c + 0x640);
        s16 ang = s[0];
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        s16 r = (s16)(0 - ang);
        if (P < r) r = P;
        if (-P > r) r = (s16)(-P);
        d[0] = r;
    }
}
""", "ptr_walk")

# Original tip clamp (r > P) which is more natural for movgt with r in r0
score(head + """\
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
""", "natural_clamp")

# Try: volatile zero to force r2?
score(head + """\
    {
        volatile int z = 0;
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * (0x2aaa + ((s64)z << 32)) + 0x800) >> 12);
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "vol_z")

# The nuclear option: split into two functions? no.

# What if P is computed as unsigned and then cast, and clamp uses different form
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        s16 ang = *(s16*)(c + 0x92);
        unsigned t = (unsigned)(spd >> 5);
        if ((int)t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)(int)t * 0x2aaaU + 0x800) >> 12);
        s16 r = (s16)(0 - ang);
        if (r > P) r = P;
        if (r < (s16)(0 - P)) r = (s16)(0 - P);
        *(s16*)(c + 0x8c) = r;
    }
}
""", "unsigned_t")

# Match ROM: rsb r0,r0,#0 then sxth via lsl/asr - force via int intermediate
score(head + """\
    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        int spd = *(int*)(c + 0x640);
        int ang = *(s16*)(c + 0x92);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        int P = (int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        P = (int)(s16)P;
        int r = 0 - ang;
        r = (int)(s16)r;
        if (r > P) r = P;
        if (r < -P) r = -P;
        r = (int)(s16)r;
        *(s16*)(c + 0x8c) = (s16)r;
    }
}
""", "all_int")
