#!/usr/bin/env python3
"""Generate particle-block variants for St_BurnFire_Main and score them."""
import subprocess, pathlib, re, textwrap, tempfile, os, shutil

REPO = pathlib.Path('.').resolve()
PY = REPO / '.venv/bin/python3'
TIP = (REPO / 'work/St_BurnFire_Main_tip.cpp').read_text()

# Isolate the particle block between the u64 +=2 and the switch
START = '    *(u8 *)(((long long)(int)(c + 0x6e5)) & 0xFFFFFFFFFFFFFFFFLL) += 2;\n'
END_MARK = '    switch (*(u8 *)(c + 0x6e3)) {'
assert START in TIP and END_MARK in TIP
head, rest = TIP.split(START, 1)
mid, tail = rest.split(END_MARK, 1)

VARIANTS = {}

# v0: original tip
VARIANTS['tip'] = mid

# v1: load order z,x,y
VARIANTS['zxy'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int xx = *(int *)(c + 0x5c);
        int yy = *(int *)(c + 0x60);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v2: precompute yy+ and zz- then natural store order
VARIANTS['precomp'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64) - 0x1e000;
        int yy = *(int *)(c + 0x60) + 0x32000;
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[1] = yy;
        info.pos[2] = zz;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v3: load x,y,z natural + store order matching ROM assignment order
VARIANTS['xyz_romstore'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int xx = *(int *)(c + 0x5c);
        int yy = *(int *)(c + 0x60);
        int zz = *(int *)(c + 0x64);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v4: y-first precompute (yy live longer?)
VARIANTS['y_first'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int yy = *(int *)(c + 0x60);
        int zz = *(int *)(c + 0x64);
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v5: separate temps for adjusted
VARIANTS['adj_temps'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);
        int pz = zz - 0x1e000;
        int py = yy + 0x32000;

        info.pos[0] = xx;
        info.pos[2] = pz;
        info.pos[1] = py;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v6: inline loads into pos assigns (no named locals for coords)
VARIANTS['inline_loads'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        info.pos[0] = *(int *)(c + 0x5c);
        info.pos[2] = *(int *)(c + 0x64) - 0x1e000;
        info.pos[1] = *(int *)(c + 0x60) + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v7: z,y,x loads; store x,y,z natural
VARIANTS['zyx_natstore'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[1] = yy + 0x32000;
        info.pos[2] = zz - 0x1e000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v8: like BurnLava (yy without add first then add on store) - already tip
# v9: force y into higher reg via volatile load of y
VARIANTS['vol_y'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(volatile int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v10: volatile x (lower?)
VARIANTS['vol_x'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(volatile int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v11: Vector3 pos + Vector3_16f vec separate
VARIANTS['vec3_types'] = '''
    {
        struct Vector3 { int x, y, z; };
        struct Vector3_16f { s16 x, y, z; };
        Vector3 pos;
        Vector3_16f vec;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);
        pos.x = xx;
        pos.z = zz - 0x1e000;
        pos.y = yy + 0x32000;
        vec.x = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        vec.y = 0;
        vec.z = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, pos.x, pos.y, pos.z, (s16 *)&vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, pos.x, pos.y, pos.z, (s16 *)&vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v12: int pos[3] first then vec (flip stack layout)
VARIANTS['pos_first'] = '''
    {
        struct Info {
            int pos[3];
            s16 vec[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v13: scalar args only, no stack pos (pass computed values)
VARIANTS['scalar_args'] = '''
    {
        s16 vec[3];
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);
        int px = xx;
        int py = yy + 0x32000;
        int pz = zz - 0x1e000;
        vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        vec[1] = 0;
        vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, px, py, pz, vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, px, py, pz, vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v14: load z y x, store using ROM order via pos[0],pos[2],pos[1] with yy computed first into named py
VARIANTS['py_early'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int py = *(int *)(c + 0x60) + 0x32000;
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = py;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v15: cast-through array for pos like 6n launder form
VARIANTS['arr_cast'] = '''
    {
        s16 vec[3];
        int pos[3];
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);
        ((int *)pos)[0] = xx;
        ((int *)pos)[2] = zz - 0x1e000;
        ((int *)pos)[1] = yy + 0x32000;
        vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        vec[1] = 0;
        vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, pos[0], pos[1], pos[2], vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, pos[0], pos[1], pos[2], vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v16: like lava but add on yy when assigning with RMW style
VARIANTS['yy_rmw'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);
        yy = yy + 0x32000;
        zz = zz - 0x1e000;

        info.pos[0] = xx;
        info.pos[2] = zz;
        info.pos[1] = yy;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v17: reverse store order pos1, pos2, pos0  
VARIANTS['store_yzx'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);

        info.pos[1] = yy + 0x32000;
        info.pos[2] = zz - 0x1e000;
        info.pos[0] = xx;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v18: interleave load with store like ROM schedule: load z, load y, load x, sub z, add y, store x, store z, store y
# try forcing with statement order + no temps for sub/add
VARIANTS['sched_rom'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        info.pos[2] = *(int *)(c + 0x64);
        info.pos[1] = *(int *)(c + 0x60);
        info.pos[0] = *(int *)(c + 0x5c);
        info.pos[2] = info.pos[2] - 0x1e000;
        info.pos[1] = info.pos[1] + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v19: comma operator to force eval order z then y then x then compute
VARIANTS['comma'] = '''
    {
        struct Info {
            s16 vec[3];
            int pos[3];
        } info;
        int zz, yy, xx;
        zz = *(int *)(c + 0x64), yy = *(int *)(c + 0x60), xx = *(int *)(c + 0x5c);
        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

# v20: pad s16 vec[4] to change alignment
VARIANTS['vec4'] = '''
    {
        struct Info {
            s16 vec[4];
            int pos[3];
        } info;
        int zz = *(int *)(c + 0x64);
        int yy = *(int *)(c + 0x60);
        int xx = *(int *)(c + 0x5c);

        info.pos[0] = xx;
        info.pos[2] = zz - 0x1e000;
        info.pos[1] = yy + 0x32000;
        info.vec[0] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2];
        info.vec[1] = 0;
        info.vec[2] = data_02082214[(*(u16 *)(c + 0x8e) >> 4) * 2 + 1];

        *(int *)(c + 0x628) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x628), 0xbd, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x62c) = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
            *(int *)(c + 0x62c), 0xbe, info.pos[0], info.pos[1], info.pos[2], info.vec, 0);
        *(int *)(c + 0x620) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
            *(int *)(c + 0x620), 0, 0x106, c + 0x74, 0);
    }

'''

results = []
outdir = REPO / 'work/burnfire/variants'
outdir.mkdir(parents=True, exist_ok=True)

for name, block in VARIANTS.items():
    src = head + START + block + END_MARK + tail
    path = outdir / f'{name}.cpp'
    path.write_text(src)
    r = subprocess.run(
        [str(PY), 'tools/fdiff.py', '--c', str(path),
         '--name', '_ZN6Player16St_BurnFire_MainEv',
         '--module', 'ov002', '--addr', '0x20d53ac', '--size', '0x3a4', '--quiet'],
        capture_output=True, text=True, cwd=str(REPO), timeout=120)
    line = (r.stdout + r.stderr).strip().splitlines()
    summary = line[-1] if line else 'no output'
    # parse mismatches
    m = re.search(r'mismatches=(\d+)/(\d+)', summary)
    mm = int(m.group(1)) if m else 999
    matched = 'match=True' in summary
    results.append((mm, matched, name, summary))
    print(f'{name:16s} {summary}')

results.sort()
print('\\n=== BEST ===')
for row in results[:10]:
    print(row)
