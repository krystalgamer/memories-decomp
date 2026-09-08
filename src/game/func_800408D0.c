#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object_layout.h"

typedef struct {
    s16 unk0;
    s16 next;
    s32 unk4;
    u16 flags;
    u8 padA[2];
    s32 unkC;
    u8 pad10[4];
    u16 unk14;
    u8 pad16;
    u8 tex_index;
    s16 unk18;
    s16 unk1A;
    u8 pad1C[6];
    u8 unk22;
    u8 pad23;
    void *callback;
    u8 pad28[8];
    s32 unk30;
    u8 pad34[8];
    u16 unk3C;
    u16 unk3E;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    u8 pad4C[0x10];
    u16 unk5C;
    u8 pad5E[8];
    u8 unk66;
    u8 pad67[9];
} DisplayObject;

typedef union {
    s32 word;
    struct {
        u16 x;
        u16 y;
    } h;
} Pos;

typedef union {
    u16 word;
    struct {
        u8 lo;
        u8 hi;
    } b;
} Half;

typedef struct {
    u32 tag;
    Pos pos;
    Half w;
    u16 h;
    u16 unkC;
    Half unkE;
    u32 unk10;
    u32 unk14;
    Pos size;
    u32 unk1C;
    s32 unk20;
} SpritePrim;

typedef struct {
    u32 unk0;
    u32 flag;
    u8 pad8[0x18];
    u8 out[4];
} ClipState;

extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));
extern s32 D_8009B424;

s32 func_80041F90(DisplayObject *arg0, s32 arg1, s32 arg2, u8 *arg3);
void func_80042188(SpritePrim *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

/* Submits one display object as a sprite in vertical strips of up to 64
   pixels. Fills the sprite primitive in the scratchpad at 0x1F800320 from
   the object, offsets it by the viewport origin unless bit 3 is set, runs
   the bit-2 clip test through func_80041F90 (setting the quad at 0x1F800344
   up as a 9-word packet, semi-transparent when the tag says so), then walks
   the object's width through func_80042188, advancing the position or the
   size and the texture coordinates (the page step depends on the depth bits
   of the tag) after each strip. */
void func_800408D0(DisplayObject *e, s32 tex, u16 mode16) {
    SpritePrim *p;
    ClipState *c;
    u8 *g;
    s32 step;
    s32 mode;
    s32 remaining;
    s32 tag;
    s32 fl;

    c = (ClipState *)0x1F800378;
    p = (SpritePrim *)0x1F800320;
    g = (u8 *)0x1F800344;
    step = 1;

    tag = e->unk4;
    p->tag = tag;
    if (tag & 0x2000000) {
        step = 4;
    } else if (tag & 0x1000000) {
        step = 2;
    }
    p->unkC = e->unk66;
    p->pos.word = e->unk30;
    p->unk10 = e->unk40;
    p->unk14 = e->unkC;
    fl = e->flags;
    p->unkE.word = e->unk5C;
    if ((fl & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        p->pos.h.x = p->pos.h.x - gGraphics_sViewportX;
        p->pos.h.y = p->pos.h.y - gGraphics_sViewportY;
    }
    p->h = e->unk3E;
    remaining = e->unk3C;
    c->flag = p->tag & 0x8000000;
    mode = mode16 | 0x10000;

    if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
        D_8009B424 = 0;
        if (func_80041F90(e, (s16)p->pos.h.x + e->unk18, (s16)p->pos.h.y + e->unk1A, c->out) <= 0) {
            return;
        }
        g[3] = 9;
        *(s32 *)(g + 4) = p->unk14;
        g[7] = 0x2C;
        if ((p->tag & 0x40000000) != 0) {
            SetSemiTrans(g, 1);
        }
        c->flag = 0x8000000;
        mode = mode16 | 0xF0000;
    } else if ((p->tag & 0x8000000) == 0) {
        p->unk20 = e->unk22 * 5760;
        p->unk1C = e->unk44;
        mode = mode16 | 0x30000;
        p->size.word = e->unk48;
        p->pos.h.x = p->pos.h.x + p->size.h.x;
        p->pos.h.y = p->pos.h.y + p->size.h.y;
    }

    do {
        p->w.word = 0x40;
        if (remaining < 0x40) {
            p->w.word = remaining;
        }
        if (p->unkE.b.lo + p->w.word > 0x100) {
            p->w.word = 0x100 - p->unkE.b.lo;
        }
        func_80042188(p, g, tex, mode, c->out);
        if (c->flag != 0) {
            p->pos.h.x = p->pos.h.x + p->w.word;
        } else {
            p->size.h.x = p->size.h.x - p->w.word;
        }
        if (p->unkE.b.lo + p->w.word >= 0x100) {
            p->unkC = p->unkC + step;
        }
        p->unkE.b.lo = p->unkE.b.lo + p->w.b.lo;
        remaining -= p->w.word;
    } while (remaining != 0);
}
