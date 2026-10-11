//cpp
/* arm9/Particle_Behavior -- the six effect behaviors of the particle engine,
 * plus the six free callbacks the system registers next to them, folded from
 * twelve legacy shards at .text 0x0204d0b8..0x0204d8e8. Each
 * Particle::<Name> is a POD behavior struct carrying one static Func that the
 * manager's behavior table calls per particle: the resource supplies the
 * EffectData record, the Element the per-particle state, and Func advances
 * the particle's velocity/offset accordingly. The span is bounded on both
 * sides by func_ free functions: func_0204cebc below and func_0204d8e8 above.
 * mwccarm emits .text in reverse source order, so the definitions below run
 * ROM-descending; the roster reads ROM-ascending.
 *
 *   RadiusConverge, LimitPlane, Turn, Converge, Jitter, Acceleration -- the
 *   Func of each behavior, against the typed data records in
 *   Particle__Behavior.h.
 *   func_0204d440, func_0204d294, func_0204d1b4, func_0204d150,
 *   func_0204d104, func_0204d0b8 -- the six callbacks the particle system
 *   registers alongside the behavior Funcs (func_0204a730's table).
 *
 * Member and field names are the tree's readable inferences (see
 * Particle__Behavior.h's header comment); offsets, extents and the effect
 * record sizes are ROM-proven.
 */

#include "decl_common.h"
#include "Particle__Behavior.h"
#include "math/Matrix.h"

extern "C" {
/* local extern: no header declares the sine table or these three matrix
 * entry points (checked include/math/*.h and decl_*.h). */
extern s16 data_02082214[];
void func_02052550(Matrix3x3* matrix, int sin, int cos);
void func_0205256c(Matrix3x3* matrix, int sin, int cos);
void Matrix3x3_SetRotationZ(Matrix3x3* matrix, int sin, int cos);
void MulVec3Mat3x3(const Vector3* in, const Matrix3x3* matrix, Vector3* out);
}

namespace Particle {

// @symbol _ZN8Particle12Acceleration4FuncERNS_10EffectDataEPcR7Vector3
void Acceleration::Func(EffectData& effect, char*, Vector3& velocity)
{
    velocity.x += effect.acceleration.x;
    velocity.y += effect.acceleration.y;
    velocity.z += effect.acceleration.z;
}

// @symbol _ZN8Particle6Jitter4FuncERNS_10EffectDataEPcR7Vector3
void Jitter::Func(EffectData& effect, char* particle, Vector3& velocity)
{
    Element& state = *(Element*)particle;
    u32 s;
    int r;
    int amp;

    if ((int)state.age % (int)effect.jitter.period != 0)
        return;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.xAmplitude;
    r = s >> 23;
    velocity.x += (amp * r - (amp << 8)) >> 8;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.yAmplitude;
    r = s >> 23;
    velocity.y += (amp * r - (amp << 8)) >> 8;

    s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
    data_020a4d30 = s;
    amp = effect.jitter.zAmplitude;
    r = s >> 23;
    velocity.z += (amp * r - (amp << 8)) >> 8;
}

// @symbol _ZN8Particle8Converge4FuncERNS_10EffectDataEPcR7Vector3
void Converge::Func(EffectData& effect, char* particle,
                    Vector3& acceleration)
{
    Element& state = *(Element*)particle;

    acceleration.x += effect.converge.strength
        * ((effect.converge.targetX - state.offset.x) - state.velocity.x) >> 12;
    acceleration.y += effect.converge.strength
        * ((effect.converge.targetY - state.offset.y) - state.velocity.y) >> 12;
    acceleration.z += effect.converge.strength
        * ((effect.converge.targetZ - state.offset.z) - state.velocity.z) >> 12;
}

// @symbol _ZN8Particle4Turn4FuncERNS_10EffectDataEPcR7Vector3
void Turn::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;
    Matrix3x3 matrix;
    int idx;

    switch (effect.turn.axis) {
    case 0:
        idx = effect.turn.angle >> 4;
        func_02052550(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 1:
        idx = effect.turn.angle >> 4;
        func_0205256c(&matrix, data_02082214[idx * 2],
                     data_02082214[idx * 2 + 1]);
        break;
    case 2:
        idx = effect.turn.angle >> 4;
        Matrix3x3_SetRotationZ(&matrix, data_02082214[idx * 2],
                              data_02082214[idx * 2 + 1]);
        break;
    }

    MulVec3Mat3x3(&state.offset, &matrix, &state.offset);
}

// @symbol _ZN8Particle10LimitPlane4FuncERNS_10EffectDataEPcR7Vector3
void LimitPlane::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;

    switch (effect.limitPlane.mode) {
    case 0:
        {
            int current, plane;
            plane = effect.limitPlane.position;
            current = state.basePosition.y;
            if (current < plane) {
                if (current + state.offset.y > plane) {
                    state.age = state.lifetime;
                    return;
                }
            }
            if (current > plane) {
                if (current + state.offset.y < plane)
                    state.age = state.lifetime;
            }
        }
        break;
    case 1:
        {
            int current, plane;
            plane = effect.limitPlane.position;
            current = state.basePosition.y;
            if (current < plane) {
                if (current + state.offset.y > plane) {
                    state.offset.y = plane - current;
                    state.velocity.y = -(int)(((s64)state.velocity.y
                        * effect.limitPlane.restitution + 0x800) >> 12);
                    return;
                }
            }
            if (current > plane) {
                if (current + state.offset.y < plane) {
                    state.offset.y = plane - current;
                    state.velocity.y = -(int)(((s64)state.velocity.y
                        * effect.limitPlane.restitution + 0x800) >> 12);
                }
            }
        }
        break;
    }
}

// @symbol _ZN8Particle14RadiusConverge4FuncERNS_10EffectDataEPcR7Vector3
void RadiusConverge::Func(EffectData& effect, char* particle, Vector3&)
{
    Element& state = *(Element*)particle;

    state.offset.x += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetX - state.offset.x) + 0x800) >> 12);
    state.offset.y += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetY - state.offset.y) + 0x800) >> 12);
    state.offset.z += (int)(((s64)effect.radiusConverge.strength
        * (effect.radiusConverge.targetZ - state.offset.z) + 0x800) >> 12);
}

} // namespace Particle

extern "C" {

struct T {
    short v0;       /* [0] */
    short v2;       /* [2] */
    short v4;       /* [4] */
    unsigned char b6;  /* [6] */
    unsigned char b7;  /* [7] */
};

// @symbol func_0204d440
void func_0204d440(short *out, int **pp, int t)
{
    struct T *p = (struct T *)pp[1];
    unsigned char n1 = p->b6;
    unsigned char n2 = p->b7;
    if (t < n1) {
        short a = p->v0;
        out[0x1a] = a + __aeabi_idiv(t * (p->v2 - a), n1);
        return;
    }
    if (t < n2) {
        out[0x1a] = p->v2;
        return;
    }
    {
        short a = p->v4;
        out[0x1a] = a + __aeabi_idiv((t - 0xff) * (a - p->v2), 0xff - n2);
    }
}

/* Three-stop RGB555 colour ramp entry. */
struct ColorRamp
{
    u16 startColor;
    u16 endColor;
    u8 stop1;
    u8 stop2;
    u8 stop3;
    u8 pad7;
    u16 flags;
};

struct EffectData
{
    u16 *palette;
    int pad4;
    struct ColorRamp *ramp;
};

struct Ptcl
{
    char pad0[0x3a];
    u16 color;
};

// @symbol func_0204d294
void func_0204d294(struct Ptcl *ptcl, struct EffectData *data, int age)
{
    u16 *palette = data->palette;
    struct ColorRamp *ramp = data->ramp;
    u8 stop1 = ramp->stop1;
    u8 stop2 = ramp->stop2;
    u8 stop3 = ramp->stop3;

    if (age < stop1)
    {
        ptcl->color = ramp->startColor;
        return;
    }
    if (age < stop2)
    {
        u16 hiColor = *(u16 *)((char *)palette + 0x12);
        u16 loColor = ramp->startColor;
        u16 flags = ramp->flags;
        int redHi = hiColor & 0x1f;
        int redLo = loColor & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int lerp = (int)(((unsigned int)(flags << 29)) >> 31);
        int num = age - stop1;
        int denom = stop2 - stop1;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (u16)(red | (green << 5) | (blue << 10));
        return;
    }
    if (age < stop3)
    {
        u16 loColor = *(u16 *)((char *)palette + 0x12);
        u16 hiColor = ramp->endColor;
        u16 flags = ramp->flags;
        int redLo = loColor & 0x1f;
        int redHi = hiColor & 0x1f;
        int greenLoRaw = loColor >> 5;
        int greenLo = greenLoRaw & 0x1f;
        int greenHiRaw = hiColor >> 5;
        int greenHi = greenHiRaw & 0x1f;
        int blueLoRaw = loColor >> 10;
        int blueLo = blueLoRaw & 0x1f;
        int blueHiRaw = hiColor >> 10;
        int blueHi = blueHiRaw & 0x1f;
        int lerp = (int)(((unsigned int)(flags << 29)) >> 31);
        int num = age - stop2;
        int denom = stop3 - stop2;
        if (lerp == 0)
            num = denom;
        int blueDiff = blueHi - blueLo;
        int blueMul = num * blueDiff;
        int blueStep = blueMul / denom;
        int redDiff = redHi - redLo;
        int redMul = num * redDiff;
        int redStep = redMul / denom;
        int greenDiff = greenHi - greenLo;
        int greenMul = num * greenDiff;
        int greenStep = greenMul / denom;
        int green = greenLo + greenStep;
        int red = redLo + redStep;
        int blue = blueLo + blueStep;
        ptcl->color = (u16)(red | (green << 5) | (blue << 10));
        return;
    }
    ptcl->color = ramp->endColor;
}

struct Dst
{
  char pad0[0x40];
  u16 u40;
};
struct Color555
{
  u16 r : 5;
  u16 g : 5;
  u16 b : 5;
  u16 a : 1;
};
struct ColorEntry
{
  struct Color555 color;
  u8 field2;
  u8 pad3;
  u8 lo;
  u8 hi;
};
struct Src
{
  char pad0[0xc];
  struct ColorEntry *p0c;
};

// @symbol func_0204d1b4
void func_0204d1b4(struct Dst *dst, struct Src *src, int t)
{
  struct ColorEntry *e = src->p0c;
  int lo = e->lo;
  int hi = e->hi;
  int r1;
  if (t < lo)
  {
    int rr = e->color.r;
    int gg = e->color.g;
    int c = gg - rr;
    int tmp = __aeabi_idiv(t * c, lo);
    r1 = tmp + rr;
  }
  else
    if (t < hi)
  {
    r1 = e->color.g;
  }
  else
  {
    int gg = e->color.g;
    int bb = e->color.b;
    int d = bb - gg;
    int tmp = __aeabi_idiv((t - 0xff) * d, 0xff - hi);
    r1 = tmp + bb;
  }
  u16 *p_d = (u16 *) ((unsigned long long) ((unsigned int) (&dst->u40)));
  u32 s = data_020a4d30 * 0x5eedf715u + 0x1b0cb173u;
  data_020a4d30 = s;
  int b = e->field2;
  u16 old = *p_d;
  *p_d = (u16) ((old & (~0x3e0)) |
    ((((u32) ((r1 * (0xff - ((b * (int) (s >> 0x18)) >> 8))) << 8) >> 0x10) & 0x1f) << 5));
}

}

extern "C" {
/* The shard carries `#pragma opt_strength_reduction off`; mwccarm only binds
 * it when it opens the extern block, so this helper sits in its own block. */
#pragma opt_strength_reduction off
// @symbol func_0204d150
void func_0204d150(char *r0, char *r1, int r2)
{
  u8 *t = *((u8 **) (r1 + 0x10));
  int i = 0;
  int acc;
  int count = t[8];
  u8 width;
  int new_var;
  if (count <= (new_var = 0))
  {
    return;
  }
  width = t[9];
  acc = 0;
  do
  {
    if (r2 < (acc + width))
    {
      r0[0x42] = t[i];
      return;
    }
    i++;
    acc += width;
  }
  while (i < t[8]);
}

}

extern "C" {

// @symbol func_0204d104
void func_0204d104(int obj, int src, int a)
{
    int t;
    int q;
    t = *(short*)(*(int*)(src + 0x14) + 4);
    q = (t - 0x1000) * (a - 0xff) / 255;
    *(short*)(obj + 0x34) = (short)(t + q);
}

// @symbol func_0204d0b8
void func_0204d0b8(void *p, int a, int b) {
    u16 *c = (u16 *)((char *)p + 0x40);
    u16 g = ((0xff - b) * 0x1f) / 0xff;
    *c = (*c & ~0x3e0) | ((g & 0x1f) << 5);
}

}
