#!/usr/bin/env python3
"""Sweep source variants of the av/diff block; report div counts."""
import re, subprocess, tempfile, pathlib, textwrap, uuid, os, sys
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
PY = REPO / ".venv/bin/python"
MATCH = REPO / "tools/match.py"
BASE = (REPO / "work/swing_tip.c").read_text()

# Marker region to replace: from "if (*(s16*)((char*)data_0209f4a0" through the ApproachLinear else
START = "        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {"
END = "        } else {\n            _Z15ApproachLinear2Rsss((s16*)(((int)c + 0x69c) & 0xFFFFFFFFFFFFFFFFLL), 0, 0x80);\n        }"

# Extract original block for verification
assert START in BASE and END in BASE
# We'll replace from START through END inclusive with variant blocks

VARIANTS = {}

def add(name, block):
    VARIANTS[name] = block

# Original
add("orig", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
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
        }""")

# av first, then diff
add("av_first", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 av = *(s16*)(c + 0x69c);
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
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
        }""")

# uninit decl: diff first, av second
add("uninit_diff_av", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff, av;
            s32 step;
            diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            av = *(s16*)(c + 0x69c);
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
        }""")

# uninit av first
add("uninit_av_diff", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 av, diff;
            s32 step;
            av = *(s16*)(c + 0x69c);
            diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
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
        }""")

# split d2/d4 loads
add("split_loads", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 d2 = *(s16*)(c + 0x6d2);
            s16 d4 = *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s16 diff = d2 - d4;
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
        }""")

# abs via ternary into new var (keep signed diff separate)
add("abs_ternary", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av0 = *(s16*)(c + 0x69c);
            s16 av = av0 < 0 ? (s16)-av0 : av0;
            s32 step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
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
        }""")

# s32 types for diff/av
add("s32_types", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s32 diff = (s16)(*(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4));
            s32 av = *(s16*)(c + 0x69c);
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
        }""")

# boost diff priority with self-select
add("diff_boost", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            diff = diff ? diff : diff;
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
        }""")

# if/else for step instead of ternary
add("step_ifelse", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            if (av < 0x800)
                step = (diff << 10) >> 16;
            else
                step = (diff << 9) >> 16;
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
        }""")

# volatile av load
add("vol_av", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(volatile s16*)(c + 0x69c);
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
        }""")

# abs with rsb form via int cast
add("abs_int", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s32 av = *(s16*)(c + 0x69c);
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
        }""")

# shift form as signed mul-ish: (s16)(diff * k) / 64 style - use (diff * 0x400)/0x10000
add("mul_form", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            if (av < 0x800)
                step = (s32)diff * 0x400 >> 16;
            else
                step = (s32)diff * 0x200 >> 16;
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
        }""")

# nest: compute step from loads without intermediate av name after abs
add("inline_abs", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            s32 aabs = av;
            if (aabs < 0) aabs = -aabs;
            step = (aabs < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
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
        }""")

# scope: av in nested block
add("nested_av", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s32 step;
            {
                s16 av = *(s16*)(c + 0x69c);
                if (av < 0) av = -av;
                step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            }
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
        }""")

# invert step condition av >= 0x800 first
add("invert_cmp", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av >= 0x800) ? ((diff << 9) >> 16) : ((diff << 10) >> 16);
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
        }""")

# coalescing: step = diff first then scale
add("step_is_diff", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 av = *(s16*)(c + 0x69c);
            s32 step = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            if (av < 0) av = -av;
            if (av < 0x800)
                step = (step << 10) >> 16;
            else
                step = (step << 9) >> 16;
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
        }""")

# named pointer for 0x69c and 0x6d2
add("named_ptr", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 *p69c = (s16*)(c + 0x69c);
            s16 *p6d2 = (s16*)(c + 0x6d2);
            s16 diff = p6d2[0] - p6d2[1];
            s16 av = *p69c;
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
        }""")

# pragma propagation off above function
add("pragma_prop", None)  # special: wrap file

# comma load order force
add("comma_load", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 d2, d4, av, diff;
            s32 step;
            d2 = *(s16*)(c + 0x6d2);
            d4 = *(s16*)(c + 0x6d4);
            av = *(s16*)(c + 0x69c);
            diff = d2 - d4;
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
        }""")

# sign-extend diff as s32 early: ((s32)(s16)(d2-d4))
add("sx_early", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s32 diff = (s16)(*(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4));
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? (diff << 10 >> 16) : (diff << 9 >> 16);
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
        }""")

# double use of diff to boost
add("diff_double", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = -av;
            step = (av < 0x800) ? ((diff << 10) >> 16) : ((diff << 9) >> 16);
            step = step + (diff - diff);
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
        }""")

# rsb form: av = 0 - av when negative
add("rsb_abs", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
            s32 step;
            if (av < 0) av = (s16)(0 - av);
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
        }""")

# declare step at top of case1
add("step_top", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s32 step;
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s16 av = *(s16*)(c + 0x69c);
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
        }""")

# use bit ops for abs: (av ^ (av>>15)) - (av>>15) style
add("xor_abs", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s32 av = *(s16*)(c + 0x69c);
            s32 step;
            s32 m = av >> 31;
            av = (av ^ m) - m;
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
        }""")

# 6aa pragma common_subs off
PRAGMA_COMMON = "#pragma opt_common_subs off\n"
PRAGMA_PROP = "#pragma opt_propagation off\n"
PRAGMA_STR = "#pragma opt_strength_reduction off\n"
PRAGMA_LIFE = "#pragma opt_lifetimes off\n"

def inject_pragma(src, pragma):
    # after //cpp
    if src.startswith("//cpp"):
        return "//cpp\n" + pragma + src[5:].lstrip("\n")
    return pragma + src

def replace_block(src, block):
    # find START ... END
    i = src.find(START)
    if i < 0:
        raise SystemExit("START not found")
    j = src.find(END, i)
    if j < 0:
        raise SystemExit("END not found")
    j += len(END)
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
    if "MATCH" in out and "MATCHING VERSIONS: none" not in out and "word(s) differ" not in out:
        # check for full match
        if re.search(r"1\.2/sp2p3: MATCH", out) or "MATCHING VERSIONS: 1.2/sp2p3" in out:
            return 0, out
    if m:
        return int(m.group(1)), out
    if "size differs" in out:
        m2 = re.search(r"candidate 0x([0-9a-f]+)", out)
        return 999, out
    if "compile failed" in out:
        return -1, out
    # try other patterns
    if "MATCHING VERSIONS: 1.2/sp2p3" in out or re.search(r"MATCH\b", out):
        return 0, out
    return -2, out

results = []
out_dir = REPO / "work/swing/variants"
out_dir.mkdir(parents=True, exist_ok=True)

# special pragma variants
extra = []
for pname, pr in [("p_common", PRAGMA_COMMON), ("p_prop", PRAGMA_PROP),
                  ("p_str", PRAGMA_STR), ("p_life", PRAGMA_LIFE),
                  ("p_common_prop", PRAGMA_COMMON+PRAGMA_PROP)]:
    extra.append((pname, inject_pragma(replace_block(BASE, VARIANTS["orig"]), pr)))

# also pragma + uninit
extra.append(("p_common_uninit", inject_pragma(replace_block(BASE, VARIANTS["uninit_diff_av"]), PRAGMA_COMMON)))
extra.append(("p_prop_uninit", inject_pragma(replace_block(BASE, VARIANTS["uninit_diff_av"]), PRAGMA_PROP)))
extra.append(("p_life_uninit", inject_pragma(replace_block(BASE, VARIANTS["uninit_diff_av"]), PRAGMA_LIFE)))

# uninit with step first in decl list
add("uninit_step_diff_av", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s32 step;
            s16 diff;
            s16 av;
            diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            av = *(s16*)(c + 0x69c);
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
        }""")

add("uninit_av_step_diff", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 av;
            s32 step;
            s16 diff;
            diff = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            av = *(s16*)(c + 0x69c);
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
        }""")

# force sign-extend with double cast after sub: (s16) then s32
add("double_cast", """\
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) != 0) {
            s16 t = *(s16*)(c + 0x6d2) - *(s16*)(c + 0x6d4);
            s32 diff = t;
            s16 av = *(s16*)(c + 0x69c);
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
        }""")

for name, block in list(VARIANTS.items()):
    if block is None:
        continue
    src = replace_block(BASE, block)
    path = out_dir / f"{name}.cpp"
    path.write_text(src)
    div, out = run_match(path)
    results.append((div, name, "ok" if div >= 0 else out[-200:]))
    print(f"{div:4d}  {name}", flush=True)

for name, src in extra:
    path = out_dir / f"{name}.cpp"
    path.write_text(src)
    div, out = run_match(path)
    results.append((div, name, "ok" if div >= 0 else out[-200:]))
    print(f"{div:4d}  {name}", flush=True)

results.sort(key=lambda x: (x[0] if x[0] >= 0 else 9999, x[1]))
print("\n=== BEST ===")
for div, name, _ in results[:15]:
    print(f"{div:4d}  {name}")
print("\n=== WORST/FAIL ===")
for div, name, note in results[-5:]:
    print(f"{div:4d}  {name}  {note[:80]}")
