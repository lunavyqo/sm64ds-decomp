//cpp
/* Model4 -- LoadTexAndPal and the two palette-VRAM helpers that follow it.
 *
 * Not part of src/engine/model/Model.cpp: func_020458a8 sits between that TU
 * and this run, and LoadCompressedTextureToVram begins where this run ends.
 * The name is the tree's Model class plus the numeric suffix already used
 * for a second translation unit (Juan 2026-10-11).
 *
 * mwccarm's default deferred codegen emits .text in reverse source order, so
 * the definitions are highest address first. Do not reorder.
 */

#include "Model.h"

extern "C" {
extern u32 data_020a4bcc;
extern u32 data_020a4bd8;
void Crash();
void _ZN2GX16BeginLoadTexPlttEv(void);
void _ZN2GX11LoadTexPlttEPKvjj(const void *src, u32 slot, u32 size);
void _ZN2GX14EndLoadTexPlttEv(void);
}

struct GX {
    static void BeginLoadTexPltt();                          /* _ZN2GX16BeginLoadTexPlttEv */
    static void LoadTexPltt(const void *src, u32 addr, u32 size);
    static void EndLoadTexPltt();
};

// 0x02045ad8 (0x80)
// @symbol func_02045ad8
extern "C" u32 func_02045ad8(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        Crash();

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, data_020a4bcc, size);
    _ZN2GX14EndLoadTexPlttEv();

    slot = data_020a4bcc;
    data_020a4bcc = slot + ((size + 0xf) & 0xfff0);

    return slot;
}

// 0x02045a50 (0x88)
// @symbol func_02045a50
extern "C" u32 func_02045a50(const void *src, u32 size)
{
    u32 slot;
    u32 end;

    slot = data_020a4bcc;
    end  = data_020a4bd8;

    if (slot + size > end)
        Crash();

    data_020a4bd8 -= (size + 0xf) & 0xfff0;

    _ZN2GX16BeginLoadTexPlttEv();
    _ZN2GX11LoadTexPlttEPKvjj(src, data_020a4bd8, size);
    _ZN2GX14EndLoadTexPlttEv();

    return data_020a4bd8;
}

// 0x020458e0 (0x170)
// @symbol _ZN5Model13LoadTexAndPalER8BMD_File
void Model::LoadTexAndPal(BMD_File &file)
{
    u32 i;
    u32 ret;
    u32 sz;
    BMD_Palette *p;
    u32 poff;
    u32 j;
    BMD_Texture *t;
    u32 off;

    i = 0;
    if (i < file.numTextures) {
        off = i;
        do {
            t = (BMD_Texture *)((char *)file.textures + off);
            sz = t->size;
            if (((t->flags >> 26) & 7) == 5) {
                ret = LoadCompressedTextureToVram(t->data, sz, t->data + sz);
            } else {
                ret = LoadTextureToVram(t->data, sz);
            }
            t->flags = (t->flags & ~0xffff) | ((ret >> 3) & 0xffff);
            i++;
            off += sizeof(BMD_Texture);
        } while (i < file.numTextures);
    }

    GX::BeginLoadTexPltt();

    j = 0;
    if (j < file.numPalettes) {
        poff = j;
        do {
            p = (BMD_Palette *)((char *)file.palettes + poff);
            sz = p->size;
            if (data_020a4bcc + sz > data_020a4bd8) Crash();
            if (sz <= 8) {
                GX::LoadTexPltt(p->data, data_020a4bcc, sz);
                p->vramOffset = data_020a4bcc;
                data_020a4bcc = data_020a4bcc + ((sz + 7) & 0xfff8);
            } else {
                data_020a4bd8 = data_020a4bd8 - ((sz + 0xf) & 0xfff0);
                GX::LoadTexPltt(p->data, data_020a4bd8, sz);
                p->vramOffset = data_020a4bd8;
            }
            j++;
            poff += sizeof(BMD_Palette);
        } while (j < file.numPalettes);
    }

    GX::EndLoadTexPltt();
}
