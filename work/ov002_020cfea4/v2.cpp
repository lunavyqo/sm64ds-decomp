//cpp
struct Vector3 { int x, y, z; };

extern "C" {
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void *_ZNK12WithMeshClsn14GetFloorResultEv(void *self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int AngleDiff(int a, int b);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *self);
extern void *_ZNK12WithMeshClsn13GetWallResultEv(void *self);
extern void _ZN13RaycastGroundC1Ev(void *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, const Vector3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(void *self);
extern void _ZN13RaycastGroundD1Ev(void *self);
extern int func_ov002_020cfd84(void *actor, const Vector3 *a, const Vector3 *b);
extern short data_02082214[];
}

extern "C" int func_ov002_020cfea4(char *c)
{
    Vector3 pts[8];
    char rg[0x54];
    void *fr;
    void *other;
    short *tbl;
    int y;
    int idx;
    short dz, dx;
    int z, x;

    if (*(unsigned short *)(c + 0x6a6) == 0 && (*(unsigned char *)(c + 0x6e9) & 2) != 0) {
        if (_ZNK12WithMeshClsn10IsOnGroundEv(c + 0x380) != 0) {
            fr = _ZNK12WithMeshClsn14GetFloorResultEv(c + 0x380);
            if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
                other = _ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(fr));
                if (other != 0) {
                    if (AngleDiff(*(short *)(c + 0x8e), *(short *)((char *)other + 0x8e)) >= 0x4000)
                        return 1;
                }
            }
        }
        if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x380) != 0) {
            fr = _ZNK12WithMeshClsn13GetWallResultEv(c + 0x380);
            if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
                other = _ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(fr));
                if (other != 0) return 1;
            }
        }
    }

    _ZN13RaycastGroundC1Ev(rg);

    /* ROM: load ang, table, y early; then dz then dx; compute z, x, y */
    {
        short ang = *(short *)(c + 0x8e);
        tbl = data_02082214;
        y = *(int *)(c + 0x60);
        idx = (unsigned short)(short)(ang + 0x8000) >> 4;
        dz = tbl[idx * 2 + 1];
        dx = tbl[idx * 2];
        z = *(int *)(c + 0x64) + ((int)dz << 5);
        x = *(int *)(c + 0x5c) + ((int)dx << 5);
        y = y + 0x14000;
        pts[1].x = x;
        pts[1].y = y;
        pts[1].z = z;
    }

    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pts[1], c);
    if (_ZN13RaycastGround10DetectClsnEv(rg) != 0) {
        if (*(int *)(rg + 0x44) >= *(int *)(c + 0x60) - 0x50000) {
            _ZN13RaycastGroundD1Ev(rg);
            return 1;
        }
    }

    /* ROM: build pts[2] first, then second angle lookup */
    {
        int py = *(int *)(c + 0x60);
        int p1x = pts[1].x;
        int p1z = pts[1].z;
        int y2 = py - 0xa000;
        pts[2].x = p1x;
        pts[2].y = y2;
        pts[2].z = p1z;

        short ang = *(short *)(c + 0x8e);
        y = *(int *)(c + 0x60);
        tbl = data_02082214;
        idx = (unsigned short)(short)(ang + 0x4000) >> 4;
        {
            int j = idx * 2;
            int y3 = y - 0xa000;
            dz = tbl[j + 1];
            dx = tbl[j];
            int k = 60;
            int dx60 = (short)dx * k;
            int dz60 = (short)dz * k;
            int p3x = p1x + dx60;
            int p3z = p1z + dz60;
            pts[3].x = p3x;
            pts[3].y = y3;
            pts[3].z = p3z;
            pts[4] = pts[2];
            pts[5] = pts[3];
            if (func_ov002_020cfd84(c, &pts[4], &pts[5]) != 0) {
                _ZN13RaycastGroundD1Ev(rg);
                return 1;
            }

            p3x = p1x - dx60;
            p3z = p1z - dz60;
            pts[3].x = p3x;
            pts[3].z = p3z;
            pts[6] = pts[2];
            pts[7] = pts[3];
            if (func_ov002_020cfd84(c, &pts[6], &pts[7]) != 0) {
                _ZN13RaycastGroundD1Ev(rg);
                return 1;
            }
        }
    }

    _ZN13RaycastGroundD1Ev(rg);
    return 0;
}
