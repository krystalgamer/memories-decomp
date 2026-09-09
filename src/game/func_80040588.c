#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "display_object_layout.h"
#include "display_object.h"
#include "display_object_projection.h"
#include "sprite_primitive.h"
#include "graphics_frame.h"

extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data
extern s32 D_8009B424;
extern s32 D_800E9D90[];
extern s16 D_800EFE3A[];
extern DisplayObject D_800EFE48[];

void func_80042188(SpritePrim *arg0, u8 *arg1, s32 arg2, s32 arg3, u8 *arg4);

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
    DisplayObjectCallback fn;
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
                fn = e->update;
                if (fn != (DisplayObjectCallback)0) {
                    fn((u8 *)e);
                }
                if (((e->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                     DISPLAY_OBJECT_RENDERABLE_MASK) != 0) {
                    break;
                }
                p->attribute = e->attribute;
                idx = e->tex_index;
                p->rgb = e->field_0C;
                p->xy.word = e->field_30.word;
                p->extent.word = e->field_3C.word;
                p->cxcy = e->field_40;
                p->uv.word = e->field_5C;
                mode = e->field_14 | 0x10000;
                tex = tb[idx];
                p->tpage = e->field_66;

                if ((e->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
                    p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    D_8009B424 = 0;
                    if (func_80041F90((struct ProjectionObj *)e, (s16)p->xy.h.x + (s16)e->field_18,
                                      (s16)p->xy.h.y + (s16)e->field_1A,
                                      (struct ProjectionOut *)(h + 0x20)) <= 0) {
                        break;
                    }
                    if (D_8009B424 != 0) {
                        continue;
                    }
                    g[3] = 9;
                    *(s32 *)(g + 4) = p->rgb;
                    g[7] = 0x2C;
                    if ((p->attribute & GsALON) != 0) {
                        SetSemiTrans(g, 1);
                    }
                    mode = e->field_14 | 0xF0000;
                } else if ((p->attribute & GsROTOFF) == 0) {
                    p->rotate = e->field_20.h.field_22 * 5760;
                    p->scale = e->field_44;
                    p->mxmy.word = e->field_48;
                    p->xy.h.x = p->xy.h.x + p->mxmy.h.x;
                    p->xy.h.y = p->xy.h.y + p->mxmy.h.y;
                    mode = e->field_14 | 0x30000;
                }
                func_80042188(p, g, tex, mode, h + 0x20);
                break;
            }
        } while (i >= 0);
    }
}
