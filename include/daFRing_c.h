/* daFRing_c -- Bowser's shockwaves (FIRERING).
 * Two rings, each a model plus a texture sequence, a material changer
 * and a texture transformer. mFrame counts up from 0; Behavior shocks
 * the player while it is still below 70.
 */
#ifndef DAFRING_C_H
#define DAFRING_C_H
#include "types.h"
#include "ModelAnim.h"
#include "TextureSequence.h"
#include "MaterialChanger.h"
#include "TextureTransformer.h"

#ifdef __cplusplus

#include "dActor_c.h"

struct daFRing_c : dActor_c {
    u8 pad_0d0[0x4];
    ModelAnim mModelAnim1;                     /* 0x0d4 */
    TextureSequence mTextureSequence1;         /* 0x138 */
    MaterialChanger mMaterialChanger1;         /* 0x14c */
    TextureTransformer mTextureTransformer1;   /* 0x160 */
    ModelAnim mModelAnim2;                     /* 0x174 */
    TextureSequence mTextureSequence2;         /* 0x1d8 */
    MaterialChanger mMaterialChanger2;         /* 0x1ec */
    TextureTransformer mTextureTransformer2;   /* 0x200 */
    u16 mFrame;                                /* 0x214 */
    u8 pad_216[0x2];

    virtual ~daFRing_c();

    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daFRing_c_size_must_be_0x218[
    sizeof(daFRing_c) == 0x218 ? 1 : -1];
#endif

#else

struct daFRing_c {
    u8 pad_000[0x5c];
    s32 mPosX;                                 /* 0x05c */
    s32 mPosY;                                 /* 0x060 */
    s32 mPosZ;                                 /* 0x064 */
    u8 pad_068[0x6c];
    ModelAnim mModelAnim1;                     /* 0x0d4 */
    TextureSequence mTextureSequence1;         /* 0x138 */
    MaterialChanger mMaterialChanger1;         /* 0x14c */
    TextureTransformer mTextureTransformer1;   /* 0x160 */
    ModelAnim mModelAnim2;                     /* 0x174 */
    TextureSequence mTextureSequence2;         /* 0x1d8 */
    MaterialChanger mMaterialChanger2;         /* 0x1ec */
    TextureTransformer mTextureTransformer2;   /* 0x200 */
    u16 mFrame;                                /* 0x214 */
    u8 pad_216[0x2];
};

#endif /* __cplusplus */

#endif
