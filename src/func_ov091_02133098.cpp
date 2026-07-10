//cpp
extern "C" {
extern void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void*,void*,void*,int,int,int,unsigned int);
struct M4x3 { int w[12]; };
struct Actor { char _0[0xb8]; int unkB8; };
void func_ov091_02133098(char* c){
    Actor* a = (Actor*)c;
    int sub2 = 0x20000;
    int d = *(int*)(c + 0x60) - *(int*)(c + 0x394);
    if (d <= 0x14000) { d = 0x14000; sub2 = 0; }
    int* m = *(int**)(c + 0x320);
    int sub = (int)(((long long)d * 0x60 + 0x800) >> 12);

    int m4 = m[4];
    int* b8 = (int*)(c + 0xb8);
    int rx = m4 - sub;
    int m5 = m[5];
    if (rx < 0xa000) rx = 0xa000;
    int ry = m5 - sub;
    a->unkB8 = d + 0x8c000;
    if (ry < 0xa000) ry = 0xa000;
    { int t = a->unkB8; if (t < 0x200000) t = 0x200000; a->unkB8 = t; }
    *b8 = *b8 >> 3;

    *(struct M4x3*)(c + 0x360) = *(struct M4x3*)(c + 0xf0);
    *(int*)(c + 0x384) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x388) = (*(int*)(c + 0x60) - sub2) >> 3;
    *(int*)(c + 0x38c) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(c, c + 0x338, c + 0x360, rx, d + 0x28000, ry, 0xf);
}
}
