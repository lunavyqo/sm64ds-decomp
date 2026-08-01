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

extern "C" int func_ov002_020cfea4(char *self)
{
    Vector3 pts[8];
    char rg[0x54];
    void *fr;
    void *other;
    int idx, dx, dz;
    int px, py, pz;

    if (*(unsigned short *)(self + 0x6a6) == 0 && (*(unsigned char *)(self + 0x6e9) & 2) != 0) {
        if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x380) != 0) {
            fr = _ZNK12WithMeshClsn14GetFloorResultEv(self + 0x380);
            if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
                other = _ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(fr));
                if (other != 0) {
                    if (AngleDiff(*(short *)(self + 0x8e), *(short *)((char *)other + 0x8e)) >= 0x4000)
                        return 1;
                }
            }
        }
        if (_ZNK12WithMeshClsn8IsOnWallEv(self + 0x380) != 0) {
            fr = _ZNK12WithMeshClsn13GetWallResultEv(self + 0x380);
            if (_ZNK10ClsnResult9GetClsnIDEv(fr) != -1) {
                other = _ZN5Actor10FindWithIDEj((unsigned int)_ZNK10ClsnResult9GetClsnIDEv(fr));
                if (other != 0) return 1;
            }
        }
    }

    _ZN13RaycastGroundC1Ev(rg);

    idx = (unsigned short)(short)(*(short *)(self + 0x8e) + 0x8000) >> 4;
    {
        short *tbl = data_02082214;
        py = *(int *)(self + 0x60);
        dz = tbl[idx * 2 + 1];
        dx = tbl[idx * 2];
        pz = *(int *)(self + 0x64) + (dz << 5);
        px = *(int *)(self + 0x5c) + (dx << 5);
        py = py + 0x14000;
        pts[1].x = px;
        pts[1].y = py;
        pts[1].z = pz;
    }

    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(rg, &pts[1], self);
    if (_ZN13RaycastGround10DetectClsnEv(rg) != 0) {
        if (*(int *)(rg + 0x44) >= *(int *)(self + 0x60) - 0x50000) {
            _ZN13RaycastGroundD1Ev(rg);
            return 1;
        }
    }

    /* pts[2] first */
    px = pts[1].x;
    pz = pts[1].z;
    py = *(int *)(self + 0x60) - 0xa000;
    pts[2].x = px;
    pts[2].y = py;
    pts[2].z = pz;

    idx = (unsigned short)(short)(*(short *)(self + 0x8e) + 0x4000) >> 4;
    {
        short *tbl = data_02082214;
        int yb = *(int *)(self + 0x60);
        dz = tbl[idx * 2 + 1];
        dx = tbl[idx * 2];
        py = yb - 0xa000;
        {
            int dx60 = dx * 60;
            int dz60 = dz * 60;
            pts[3].x = px + dx60;
            pts[3].y = py;
            pts[3].z = pz + dz60;

            pts[4] = pts[2];
            pts[5] = pts[3];
            if (func_ov002_020cfd84(self, &pts[4], &pts[5]) != 0) {
                _ZN13RaycastGroundD1Ev(rg);
                return 1;
            }

            pts[3].x = px - dx60;
            pts[3].z = pz - dz60;

            pts[6] = pts[2];
            pts[7] = pts[3];
            if (func_ov002_020cfd84(self, &pts[6], &pts[7]) != 0) {
                _ZN13RaycastGroundD1Ev(rg);
                return 1;
            }
        }
    }

    _ZN13RaycastGroundD1Ev(rg);
    return 0;
}
