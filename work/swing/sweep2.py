#!/usr/bin/env python3
import re, subprocess
from pathlib import Path
REPO = Path('.').resolve()
PY = REPO / ".venv/bin/python"
MATCH = REPO / "tools/match.py"
BASE = (REPO / "work/swing_tip.c").read_text()
START = "        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {"
END = "        } else {\n            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);\n        }"

def replace_block(src, block):
    i = src.find(START); j = src.find(END, i) + len(END)
    return src[:i] + block + src[j:]

def run_match(src_path):
    r = subprocess.run(
        [str(PY), str(MATCH), "--c", str(src_path),
         "--func", "_ZN6Player19St_SwingPlayer_MainEv",
         "--addr", "0x20d9fec", "--size", "0x3c4",
         "--version", "1.2/sp2p3", "--module", "ov002", "--brief"],
        capture_output=True, text=True, cwd=str(REPO), timeout=120)
    out = r.stdout + r.stderr
    m = re.search(r"(\d+) word\(s\) differ", out)
    if m: return int(m.group(1)), out
    if "size differs" in out: return 999, out
    if "compile failed" in out: return -1, out
    if "MATCHING VERSIONS: 1.2/sp2p3" in out: return 0, out
    return -2, out

CLAMPS = """\
            if (step != 0) {
                *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL) += step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }"""

VARIANTS = {
# force more simultaneous live values so av spills to ip
"live_dummy": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 d2 = *(s16*)(c + 0x6d2);
            s16 d4 = *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s16 diff = d2 - d4;
            s32 step;
            s32 keep = d2; // extra live
            (void)keep;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            step = step + (keep - keep);
{CLAMPS}""",

# laundered av load
"launder_av": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# laundered both fields of diff
"launder_diff": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 diff = *(s16*)(((int)c + 0x6d2) & 0xFFFFFFFFFFFFFFFFLL)
                     - *(s16*)(((int)c + 0x6d4) & 0xFFFFFFFFFFFFFFFFLL);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# step coalesced carefully with s16 intermediate
"step_s16": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 av = *(s16*)(c + 0x69c);
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s32 step;
            if (av < 0) av = -av;
            {{
                s32 d = diff;
                step = (av < 0x800) ? ((d << 10) >> 16) : ((d << 9) >> 16);
            }}
{CLAMPS}""",

# goto-style abs with shared step calc
"goto_abs": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av >= 0) goto pos;
            av = -av;
        pos:
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# reverse sub order then negate: d4-d2, -diff — might change regs
"rev_sub": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 diff = *(s16*)(c + 0x6d4) - *(s16*)(c + 0x6d2);
            diff = -diff;
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# use unsigned for av abs comparison
"u_av": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = ((u32)av < 0x800u) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# explicit sign extend of diff via (diff << 16) >> 16 first
"sx_named": f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            s32 raw = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 diff = (raw << 16) >> 16;
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
{CLAMPS}""",

# move arr[3] decl into this block only (already case-scoped) — put temps before if
"temps_before": None,

# C not cpp? try without //cpp - might change
}

# Also try whole-file changes: move Vec3/arr relative order
BASE2 = BASE.replace(
    "    case 1: {\n        int arr[3];\n        Vec3 v;",
    "    case 1: {\n        Vec3 v;\n        int arr[3];",
)
BASE3 = BASE.replace(
    "    case 1: {\n        int arr[3];\n        Vec3 v;",
    "    case 1: {\n        int arr[3];",
).replace(
    "            if (a2 >= 0xf99) {\n                u32 id = 0x132;\n                v.x",
    "            if (a2 >= 0xf99) {\n                Vec3 v;\n                u32 id = 0x132;\n                v.x",
)

# try declare diff as int at function scope (C89 top of function)
# inject after opening brace of function
FUNC_INJECT = """
    s16 g_diff;
    s16 g_av;
"""

out_dir = REPO / "work/swing/variants2"
out_dir.mkdir(parents=True, exist_ok=True)

results = []
for name, block in VARIANTS.items():
    if block is None: continue
    src = replace_block(BASE, block)
    path = out_dir / f"{name}.cpp"
    path.write_text(src)
    div, _ = run_match(path)
    results.append((div, name))
    print(f"{div:4d}  {name}", flush=True)

# base rearrangements
for name, src in [("swap_arr_v", BASE2), ("v_local", BASE3)]:
    path = out_dir / f"{name}.cpp"
    path.write_text(src)
    div, _ = run_match(path)
    results.append((div, name))
    print(f"{div:4d}  {name}", flush=True)

# function-scope uninit
src = BASE.replace(
    "extern \"C\" int _ZN6Player19St_SwingPlayer_MainEv(char* c)\n{\n",
    "extern \"C\" int _ZN6Player19St_SwingPlayer_MainEv(char* c)\n{\n    s16 f_diff; s16 f_av; s32 f_step;\n",
)
# and use f_* in the block
block = f"""\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {{
            f_diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            f_av = *(s16*)(c + 0x69c);
            if (f_av < 0) f_av = -f_av;
            f_step = (f_av < 0x800) ? ((f_diff << 10) >> 16) : ((f_diff << 9) >> 16);
            {{
                s32 step = f_step;
{CLAMPS}"""
# CLAMPS has wrong nesting - fix manually
block = """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            f_diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            f_av = *(s16*)(c + 0x69c);
            if (f_av < 0) f_av = -f_av;
            f_step = (f_av < 0x800) ? ((f_diff << 10) >> 16) : ((f_diff << 9) >> 16);
            if (f_step != 0) {
                *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL) += f_step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }"""
src = replace_block(src, block)
path = out_dir / "func_scope.cpp"
path.write_text(src)
div, _ = run_match(path)
results.append((div, "func_scope"))
print(f"{div:4d}  func_scope", flush=True)

# try plain C (no //cpp)
src_c = BASE.replace("//cpp\n", "", 1)
# change to .c and -lang might be forced by match from content
path = out_dir / "plain_c.c"
# match auto-detects //cpp; without it uses c99. But we have C++ mangled names and extern C - should work as C if no C++ features
# Vec3 with volatile is fine in C
path.write_text(src_c)
div, out = run_match(path)
results.append((div, "plain_c"))
print(f"{div:4d}  plain_c", flush=True)

# try: abs of av as expression in the ternary only (no separate abs)
block = """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            step = (((av < 0) ? -av : av) < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            if (step != 0) {
                *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL) += step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }"""
path = out_dir / "inline_abs_expr.cpp"
path.write_text(replace_block(BASE, block))
div, _ = run_match(path)
results.append((div, "inline_abs_expr"))
print(f"{div:4d}  inline_abs_expr", flush=True)

# try asm-like: compute with int and force register via register keyword (likely ignored)
block = """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            register s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            register s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            if (step != 0) {
                *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL) += step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }"""
path = out_dir / "register_kw.cpp"
path.write_text(replace_block(BASE, block))
div, _ = run_match(path)
results.append((div, "register_kw"))
print(f"{div:4d}  register_kw", flush=True)

# multi-assign: s16 d2,d4; s16 av; then assign interleaved with cmp start
block = """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 d2 = *(s16*)(c + 0x6d2);
            s16 d4 = *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step = d2 - d4;
            if (av < 0) av = -av;
            if (av < 0x800)
                step = (s16)step << 10 >> 16;
            else
                step = (s16)step << 9 >> 16;
            if (step != 0) {
                *(s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL) += step;
                if (*(s16*)(c + 0x69c) >= 0x1800)
                    *(s16*)(c + 0x69c) = 0x1800;
                else if (*(s16*)(c + 0x69c) <= -0x1800)
                    *(s16*)(c + 0x69c) = -0x1800;
            } else {
                _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x20);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);
        }"""
path = out_dir / "step_is_diff_s16cast.cpp"
path.write_text(replace_block(BASE, block))
div, _ = run_match(path)
results.append((div, "step_is_diff_s16cast"))
print(f"{div:4d}  step_is_diff_s16cast", flush=True)

results.sort()
print("\n=== BEST ===")
for d,n in results[:12]:
    print(f"{d:4d}  {n}")
