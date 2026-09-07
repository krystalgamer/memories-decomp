#include "../types.h"
#include "display_object_layout.h"

typedef void (*ObjFn)(void *);

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
    ObjFn callback;
    u8 pad28[8];
    s32 unk30;
    u8 pad34[8];
    s32 unk3C;
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

typedef struct {
    u32 tag;
    Pos pos;
    u32 unk8;
    u16 unkC;
    u16 unkE;
    u32 unk10;
    u32 unk14;
    Pos size;
    u32 unk1C;
    s32 unk20;
} SpritePrim;

extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));
extern s32 D_8009B424;
extern s32 D_800E9D90[];
extern s16 D_800EFE3A[];
extern DisplayObject D_800EFE48[];

s32 func_80041F90(DisplayObject *arg0, s32 arg1, s32 arg2, u8 *arg3);
void func_80042188(SpritePrim *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);
extern void SetSemiTrans(void *, int);

/* Walks the display-object list rooted at D_800EFE3A: calls each object's
   callback, and for every renderable object fills the sprite primitive in
   the scratchpad at 0x1F800320 from the object, offsets it by the viewport
   origin unless bit 3 is set, and submits it through func_80042188. With
   bit 2 the position first goes through the clip test in func_80041F90,
   which may ask (D_8009B424) for the object to be run again, and the quad
   at 0x1F800344 is set up as a 9-word semi-transparent packet instead. */
void func_80040588(void) {
    SpritePrim *p;
    u8 *g;
    u8 *h;
    DisplayObject *e;
    s32 *tb;
    ObjFn fn;
    s32 i;
    s32 tex;
    s32 mode;
    s32 idx;

    i = D_800EFE3A[0];
    if (i >= 0) {
        g = (u8 *)0x1F800344;
        p = (SpritePrim *)0x1F800320;
        h = (u8 *)0x1F800378;
        tb = D_800E9D90;

        do {
            e = (DisplayObject *)((u8 *)D_800EFE48 + i * DISPLAY_OBJECT_RECORD_SIZE);
            i = e->next;
            while (1) {
                fn = e->callback;
                if (fn != (ObjFn)0) {
                    fn(e);
                }
                if (((e->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                     DISPLAY_OBJECT_RENDERABLE_MASK) != 0) {
                    break;
                }
                p->tag = e->unk4;
                idx = e->tex_index;
                p->unk14 = e->unkC;
                p->pos.word = e->unk30;
                p->unk8 = e->unk3C;
                p->unk10 = e->unk40;
                p->unkE = e->unk5C;
                mode = e->unk14 | 0x10000;
                tex = tb[idx];
                p->unkC = e->unk66;

                if ((e->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    p->pos.h.x = p->pos.h.x - gGraphics_sViewportX;
                    p->pos.h.y = p->pos.h.y - gGraphics_sViewportY;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    D_8009B424 = 0;
                    if (func_80041F90(e, (s16)p->pos.h.x + e->unk18,
                                      (s16)p->pos.h.y + e->unk1A, h + 0x20) <= 0) {
                        break;
                    }
                    if (D_8009B424 != 0) {
                        continue;
                    }
                    g[3] = 9;
                    *(s32 *)(g + 4) = p->unk14;
                    g[7] = 0x2C;
                    if ((p->tag & 0x40000000) != 0) {
                        SetSemiTrans(g, 1);
                    }
                    mode = e->unk14 | 0xF0000;
                } else if ((p->tag & 0x8000000) == 0) {
                    p->unk20 = e->unk22 * 5760;
                    p->unk1C = e->unk44;
                    p->size.word = e->unk48;
                    p->pos.h.x = p->pos.h.x + p->size.h.x;
                    p->pos.h.y = p->pos.h.y + p->size.h.y;
                    mode = e->unk14 | 0x30000;
                }
                func_80042188(p, g, tex, mode, h + 0x20);
                break;
            }
        } while (i >= 0);
    }
}
