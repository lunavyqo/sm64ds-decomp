//cpp
// NONMATCHING: dust matrix extract load order (ldr #0x24/#0x28 vs ROM #0x28/#0x24) (div=4)
#pragma opt_propagation off
struct Vector3 { int x, y, z; };
struct Mat4x3 { int m[12]; };

struct C;
typedef int (C::*PMF)();
struct C { char pad[0x37c]; PMF *pp; };

extern "C" {
extern void *data_0209f318;
extern Mat4x3 data_020a0e68;
extern Vector3 data_ov073_02123040;
extern char data_ov073_02123320[];
extern char data_ov073_02123330[];
extern char data_ov073_02123340[];
extern char data_ov073_02123350[];
extern char data_ov073_02123360[];
extern char data_ov073_02123380[];
extern char data_ov073_02123390[];
extern char data_ov073_021233a0[];
extern char data_ov073_021233c0[];
extern char data_ov073_021233d0[];
extern char data_ov073_021233f0[];
extern char data_ov073_02123400[];

extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, void *clsn);
extern int _ZNK9Animation12WillHitFrameEi(void *self, int f);
extern void MulMat4x3Mat4x3(void *d, void *a, void *b);
extern void Vec3_Lsl(Vector3 *d, Vector3 *s, int sh);
extern void func_02012694(int a, void *b);
extern void _ZN5Actor17HugeLandingDustAtER7Vector3b(void *self, Vector3 *v, int b);
extern void _ZN11RaycastLineC1Ev(void *self);
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, int angX);
extern void MulVec3Mat4x3(void *a, void *m, void *out);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(void *self, const Vector3 *a, const Vector3 *b, void *actor);
extern int _ZN11RaycastLine10DetectClsnEv(void *self);
extern void _ZN11RaycastLineD1Ev(void *self);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, void *wmc, unsigned int flags);
extern void _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(void *self, const Vector3 *v);
extern void func_ov073_0211f61c(void *self);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov073_021215cc(void *self);
extern void _ZN14BlendModelAnim7AdvanceEv(void *self);
}

extern "C" int _ZN11ChiefChilly8BehaviorEv(C *c)
{
    char *self = (char *)c;
    Vector3 v0;
    Vector3 start, end, vin, vout;
    Vector3 v3C;
    Vector3 v48;
    Vector3 v54;
    char line[0x7c];

    *(C **)((char *)data_0209f318 + 0x114) = c;
    DecIfAbove0_Short((unsigned short *)(self + 0x100));

    if (*(void **)((char *)c->pp + 8) != 0) {
        PMF *p = c->pp + 1;
        (c->**p)();
    }

    if ((char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340) {
        _ZN5Actor9UpdatePosEP12CylinderClsn(self, self + 0x110);
    } else {
        int sum = *(int *)(self + 0xa8) + *(int *)(self + 0x9c);
        int m = *(int *)(self + 0xa0);
        int ac = *(int *)(self + 0xac);
        if (sum >= m)
            m = sum;
        *(int *)(self + 0xa8) = m;
        *(int *)(self + 0xac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(self, self + 0x110);
    }

    if (((char *)c->pp == data_ov073_02123330 || (char *)c->pp == data_ov073_02123350)
        && _ZNK9Animation12WillHitFrameEi(self + 0x35c, 7) != 0) {
        data_020a0e68 = *(Mat4x3 *)(self + 0x328);
        MulMat4x3Mat4x3(*(char **)(self + 0x320) + 0x60, &data_020a0e68, &data_020a0e68);
        v0.y = data_020a0e68.m[10];
        v0.x = data_020a0e68.m[9];
        v0.z = data_020a0e68.m[11];
        Vec3_Lsl(&v3C, &v0, 3);
        v0.x = v3C.x;
        v0.y = v3C.y;
        v0.z = v3C.z;
        func_02012694(0x167, self + 0x74);
        v48.x = v0.x;
        v48.y = v0.y;
        v48.z = v0.z;
        _ZN5Actor17HugeLandingDustAtER7Vector3b(self, &v48, 1);
    }

    if ((char *)c->pp != data_ov073_021233c0
        && (char *)c->pp != data_ov073_021233d0
        && (char *)c->pp != data_ov073_021233f0
        && (char *)c->pp != data_ov073_02123400
        && (char *)c->pp != data_ov073_02123320
        && (char *)c->pp != data_ov073_02123340
        && (char *)c->pp != data_ov073_02123380) {
        int ang;
        _ZN11RaycastLineC1Ev(line);
        {
            int z = 0;
            start.x = z; start.y = z; start.z = z;
            end.x = z; end.y = z; end.z = z;
            vin.x = z; vin.y = z; vin.z = z;
            vout.x = z; vout.y = z; vout.z = z;
        }
        start.x = *(int *)(self + 0x5c);
        ang = 0x2000;
        {
            int y = *(int *)(self + 0x60);
            start.y = y;
            int z = *(int *)(self + 0x64);
            start.y = y + 0x78000;
            start.z = z;
        }
        if (*(unsigned char *)(self + 0x4cb) > 1)
            vin.z = 0x258000;
        else
            vin.z = 0x12c000;
        Matrix4x3_FromRotationY(&data_020a0e68, *(short *)(self + 0x94));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, ang);
        MulVec3Mat4x3(&vin, &data_020a0e68, &vout);
        {
            int sx = start.x;
            int ox = vout.x;
            int sy = start.y;
            int sz = start.z;
            end.x = sx;
            end.x = sx + ox;
            {
                int oy = vout.y;
                int oz = vout.z;
                end.y = sy;
                end.y = sy + oy;
                end.z = sz;
                end.z = sz + oz;
            }
        }
        _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(line, &start, &end, self);
        if (_ZN11RaycastLine10DetectClsnEv(line) == 0) {
            if (*(int *)(self + 0x98) > 0xa000) {
                *(unsigned char *)(self + 0x4c9) = 1;
            }
            *(int *)(self + 0x4ec) = *(int *)(self + 0x5c);
            *(int *)(self + 0x4f0) = *(int *)(self + 0x60);
            *(int *)(self + 0x4f4) = *(int *)(self + 0x64);
            if ((char *)c->pp != data_ov073_021233a0) {
                *(int *)(self + 0x5c) = *(int *)(self + 0x68);
                *(int *)(self + 0x60) = *(int *)(self + 0x6c);
                *(int *)(self + 0x64) = *(int *)(self + 0x70);
                *(int *)(self + 0x98) = 0;
            }
        } else {
            *(unsigned char *)(self + 0x4c9) = 0;
        }
        _ZN11RaycastLineD1Ev(line);
    }

    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(self, self + 0x150, 0);

    {
        int x = data_ov073_02123040.x;
        int y = data_ov073_02123040.y;
        int z = data_ov073_02123040.z;
        v54.x = x;
        v54.z = z;
        v54.y = y;
    }
    _ZN25MovingCylinderClsnWithPos21SetPosRelativeToActorERK7Vector3(self + 0x110, &v54);

    if ((char *)c->pp == data_ov073_02123360
        || (char *)c->pp == data_ov073_02123390) {
        func_ov073_0211f61c(self);
    }
    _ZN12CylinderClsn5ClearEv(self + 0x110);
    _ZN12CylinderClsn6UpdateEv(self + 0x110);
    func_ov073_021215cc(self);
    _ZN14BlendModelAnim7AdvanceEv(self + 0x30c);
    return 1;
}
