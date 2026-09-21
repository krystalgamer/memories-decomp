#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "display_object_render_sprite_list.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "ordering_tables.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"

extern u8 tail_data_start[];

void DisplayObject_UpdateResourceVariant(
    DisplayObjectConfig *object,
    s32 value
)
{
    if (object->field_69 != value) {
        object->field_69 = value;
        object->flags &= 0xFFEF;
    }
}

void DisplayObject_SetResourcePath(
    DisplayObjectConfig *object,
    u8 field_67,
    u8 field_68,
    u8 field_69
)
{
    object->field_67 = field_67;
    object->field_68 = field_68;
    object->field_69 = field_69;
    object->flags &= 0xFFEF;
}

void *DisplayObject_ConfigureSpriteResource(DisplayObject *configured, int field_67, int field_68,
                    int field_69, int color, int texture)
{
    u16 flags;

    configured->field_67 = field_67;
    configured->field_68 = field_68;
    configured->field_69 = field_69;
    configured->field_66 = color;
    *(u8 *)&configured->field_5E = color >> 16;
    *((u8 *)&configured->field_5E + 1) = color >> 8;
    configured->field_40.h.field_40 = texture & 0x3F0;
    configured->field_40.h.field_42 = (texture & 0xF) + 0xF0;
    flags = configured->flags & ~DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    configured->flags = flags;
    if (texture & 0x8000) {
        configured->flags = flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    }
    return configured;
}

void DisplayObject_ConfigureSpriteAtPosition(void *object, s32 x, s32 y, s32 field_67, s32 field_68,
                   s32 field_69, s32 color, s32 texture)
{
    DisplayObject *o = object;

    o->field_30.h.field_30 = x;
    o->field_30.h.field_32 = y;
    DisplayObject_ConfigureSpriteResource(object, field_67, field_68, field_69, color, texture);
}

DisplayObject *DisplayObject_ConfigureScreenSprite(
    DisplayObject *object,
    s32 x,
    s32 y,
    s32 height,
    s32 width,
    s32 field_5C,
    s32 field_5D,
    s32 field_66,
    s32 field_40,
    s32 field_42)
{
    s32 half_height;
    s32 half_width;

    object->field_3C.h.field_3C = height;
    half_height = height / 2;
    object->field_30.h.field_30 = x;
    object->field_30.h.field_32 = y;
    object->field_18 = half_height;
    object->field_48.h.field_48 = half_height;

    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;

    object->field_3C.h.field_3E = width;
    half_width = width / 2;
    *(u8 *)&object->field_5C = field_5C;
    ((u8 *)&object->field_5C)[1] = field_5D;
    object->field_66 = field_66;
    object->field_40.h.field_40 = field_40;
    object->field_40.h.field_42 = field_42;
    object->field_1A = half_width;
    object->field_48.h.field_4A = half_width;
    return object;
}

/* Walks the display-object list rooted at D_800EFE3A: calls each object's
   callback, and for every renderable object fills the sprite primitive in
   the scratchpad at 0x1F800320 from the object, offsets it by the viewport
   origin unless bit 3 is set, and submits it through DisplayObject_SubmitPacket. With
   bit 2 the position first goes through the clip test in func_80041F90,
   which may ask (D_8009B424) for the object to be run again, and the
   POLY_FT4 at 0x1F800344 is set up as a 9-word semi-transparent packet
   instead. */
void DisplayObject_RenderSpriteList(void) {
    SpritePrim *p;
    POLY_FT4 *g;
    ClipState *h;
    DisplayObject *e;
    GsOT **tb;
    DisplayObjectCallback fn;
    s32 i;
    GsOT *ot;
    s32 mode;
    s32 idx;

    i = D_800EFE3A[0];
    if (i >= 0) {
        g = (POLY_FT4 *)0x1F800344;
        p = (SpritePrim *)0x1F800320;
        h = (ClipState *)0x1F800378;
        tb = D_800E9D90;

        do {
            e = (DisplayObject *)((u8 *)D_800EFE48 + i * DISPLAY_OBJECT_RECORD_SIZE);
            i = e->next;
            while (1) {
                fn = e->update;
                if (fn != (DisplayObjectCallback)0) {
                    fn((u8 *)e);
                }
                if (((e->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                     DISPLAY_OBJECT_RENDERABLE_MASK) != 0) {
                    break;
                }
                p->attribute = e->attribute;
                idx = e->ot_index;
                p->rgb = e->field_0C;
                p->xy.word = e->field_30.word;
                p->extent.word = e->field_3C.word;
                p->cxcy.word = e->field_40.word;
                p->uv.word = e->field_5C;
                mode = e->field_14 | 0x10000;
                ot = tb[idx];
                p->tpage = e->field_66;

                if ((e->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
                    p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    D_8009B424 = 0;
                    if (func_80041F90(e, (s16)p->xy.h.x + (s16)e->field_18,
                                      (s16)p->xy.h.y + (s16)e->field_1A,
                                      (struct ProjectionOut *)h->out) <= 0) {
                        break;
                    }
                    if (D_8009B424 != 0) {
                        continue;
                    }
                    setlen(g, 9);
                    *(s32 *)&g->r0 = p->rgb;
                    g->code = 0x2C;
                    if ((p->attribute & GsALON) != 0) {
                        SetSemiTrans(g, 1);
                    }
                    mode = e->field_14 | 0xF0000;
                } else if ((p->attribute & GsROTOFF) == 0) {
                    p->rotate = e->field_20.h.field_22 * 5760;
                    p->scale = e->field_44.word;
                    p->mxmy.word = e->field_48.word;
                    p->xy.h.x = p->xy.h.x + p->mxmy.h.x;
                    p->xy.h.y = p->xy.h.y + p->mxmy.h.y;
                    mode = e->field_14 | 0x30000;
                }
                DisplayObject_SubmitPacket(p, (u8 *)g, (s32)ot, mode, h->out);
                break;
            }
        } while (i >= 0);
    }
}
