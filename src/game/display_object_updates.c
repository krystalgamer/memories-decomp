#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "sprite_primitive.h"
#include "graphics_frame.h"
#include "display_object_updates.h"

#include "ordering_tables.h"
#define FUNC_80042188_SPRITE_VIEW
#include "../unmatched.h"
#undef FUNC_80042188_SPRITE_VIEW

extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data

/* Submits one display object as a sprite in vertical strips of up to 64
   pixels. Fills the sprite primitive in the scratchpad at 0x1F800320 from
   the object, offsets it by the viewport origin unless bit 3 is set, runs
   the bit-2 clip test through func_80041F90 (setting the quad at 0x1F800344
   up as a 9-word packet, semi-transparent when the tag says so), then walks
   the object's width through func_80042188, advancing the position or the
   size and the texture coordinates (the page step depends on the depth bits
   of the tag) after each strip. */
void func_800408D0(DisplayObject *e, s32 tex, s32 mode_arg) {
    u16 mode16 = mode_arg;
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

    tag = e->attribute;
    p->attribute = tag;
    if (tag & DISPLAY_OBJECT_ATTRIBUTE_16BPP) {
        step = 4;
    } else if (tag & DISPLAY_OBJECT_ATTRIBUTE_8BPP) {
        step = 2;
    }
    p->tpage = e->field_66;
    p->xy.word = e->field_30.word;
    p->cxcy.word = e->field_40.word;
    p->rgb = e->field_0C;
    fl = e->flags;
    p->uv.word = e->field_5C;
    if ((fl & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
        p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
    }
    p->extent.wh.h = e->field_3C.h.field_3E;
    remaining = e->field_3C.h.field_3C;
    c->flag = p->attribute & GsROTOFF;
    mode = mode16 | 0x10000;

    if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
        D_8009B424 = 0;
        if (func_80041F90(e, (s16)p->xy.h.x + (s16)e->field_18,
                          (s16)p->xy.h.y + (s16)e->field_1A,
                          (struct ProjectionOut *)c->out) <= 0) {
            return;
        }
        g[3] = 9;
        *(s32 *)(g + 4) = p->rgb;
        g[7] = 0x2C;
        if ((p->attribute & GsALON) != 0) {
            SetSemiTrans(g, 1);
        }
        c->flag = GsROTOFF;
        mode = mode16 | 0xF0000;
    } else if ((p->attribute & GsROTOFF) == 0) {
        p->rotate = e->field_20.h.field_22 * 5760;
        p->scale = e->field_44.word;
        mode = mode16 | 0x30000;
        p->mxmy.word = e->field_48.word;
        p->xy.h.x = p->xy.h.x + p->mxmy.h.x;
        p->xy.h.y = p->xy.h.y + p->mxmy.h.y;
    }

    do {
        p->extent.wh.w.word = 0x40;
        if (remaining < 0x40) {
            p->extent.wh.w.word = remaining;
        }
        if (p->uv.b.lo + p->extent.wh.w.word > 0x100) {
            p->extent.wh.w.word = 0x100 - p->uv.b.lo;
        }
        func_80042188(p, g, tex, mode, c->out);
        if (c->flag != 0) {
            p->xy.h.x = p->xy.h.x + p->extent.wh.w.word;
        } else {
            p->mxmy.h.x = p->mxmy.h.x - p->extent.wh.w.word;
        }
        if (p->uv.b.lo + p->extent.wh.w.word >= 0x100) {
            p->tpage = p->tpage + step;
        }
        p->uv.b.lo = p->uv.b.lo + p->extent.wh.w.b.lo;
        remaining -= p->extent.wh.w.word;
    } while (remaining != 0);
}

void func_80040BF8(void)
{
    s32 i = D_800EFE38[3];

    if (i >= 0) {
        DisplayObject *base = D_800EFE48;
        GsOT **table = D_800E9D90;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = (u8 *)object;

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                func_800408D0(object, (s32)table[object->ot_index],
                              *(s16 *)(data + 0x14));
            }
        } while (i >= 0);
    }
}

void func_80040CAC(void)
{
    s32 i = D_800EFE38[0];

    while (i >= 0) {
        DisplayObject *object = &D_800EFE48[i];
        DisplayObjectCallback callback = object->update;

        i = object->next;
        if (callback != 0) {
            callback((u8 *)object);
        }
    }
}

void func_80040D14(void)
{
    s32 i = D_800EFE38[6];

    if (i >= 0) {
        DisplayObject *base = D_800EFE48;
        GsOT **table = D_800E9D90;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = (u8 *)object;

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                void (*secondary)(u8 *, s32) =
                    *(void (**)(u8 *, s32))(data + 0x4C);

                if (secondary != 0) {
                    secondary(data, (s32)table[object->ot_index]);
                }
            }
        } while (i >= 0);
    }
}
