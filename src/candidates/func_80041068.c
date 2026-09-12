/* Reclassified from matching_c (#3859). This was part of
 * src/game/display_object_list_renderers.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0, a single threshold,
 * it is 182 of 182 instructions with 12 differing, opcode distance 0. The
 * source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../game/graphics_frame.h"
#include "../game/display_object_projection.h"
#include "../game/display_object.h"
#include "../game/display_object_layout.h"
#define FUNC_80042188_RAW_ATTRIBUTE_VIEW
#include "../game/display_object_list_renderers.h"
#define func_80042188 func_80042188_default_view
#include "../unmatched.h"
#undef func_80042188

#include "../game/ordering_tables.h"

/* The POLY_GT4 sibling for the list rooted at D_800EFE38[5]: the same walk,
 * with each vertex carrying a texture coordinate as well. Its second
 * submission, gated on the byte at 0x72 two bytes past the record, rewrites
 * the first two vertices from 0x58; display_object.h records why that reach
 * stays a byte reach. */
void func_80041068(void) {
    POLY_GT4 *g;
    u8 *h;
    DisplayObject *e;
    GsOT **tb;
    DisplayObjectCallback fn;
    s32 twelve;
    s32 hi;
    s32 bit;
    s32 v;
    s32 i;
    s32 w0;
    s32 w1;
    s32 w2;
    s32 w3;
    s32 w4;
    s32 fl;
    s32 dx;
    s32 x0;
    s32 x1;
    s32 x2;

    g = (POLY_GT4 *)0x1F800344;
    h = (u8 *)0x1F800398;
    i = D_800EFE38[5];

    if (i >= 0) {
        twelve = 0xC;
        hi = 0x3C;
        tb = D_800E9D90;
        bit = 0x50000;

        do {
            e = &D_800EFE48[i];
            fn = e->update;
            i = e->next;
            if (fn != (DisplayObjectCallback)0) {
                fn((u8 *)e);
            }
            if (((e->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                v = e->attribute;
                w0 = e->position.word;
                w1 = e->field_34.word;
                w2 = e->field_40.word;
                w3 = e->field_4C;
                *(s32 *)&g->x0 = w0;
                *(s32 *)&g->x1 = w1;
                *(s32 *)&g->x2 = w2;
                *(s32 *)&g->x3 = w3;
                *(s32 *)&g->r0 = *(s32 *)&e->field_2C;
                *(s32 *)&g->r1 = *(s32 *)&e->field_38;
                *(s32 *)&g->r2 = *(s32 *)&e->field_44;
                *(s32 *)&g->r3 = *(s32 *)&e->field_50;
                *(u16 *)&g->u0 = *(u16 *)&e->field_30;
                *(u16 *)&g->u1 = *(u16 *)&e->field_3C;
                *(u16 *)&g->u2 = *(u16 *)&e->field_48;
                w4 = *(u16 *)&e->field_54;
                setlen(g, twelve);
                setcode(g, hi);
                fl = e->flags;
                *(u16 *)&g->u3 = w4;

                if ((fl & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    dx = gGraphics_sViewportX;
                    g->x0 -= dx;
                    g->x1 -= dx;
                    g->x2 -= dx;
                    g->x3 -= dx;
                    dx = gGraphics_sViewportY;
                    g->y0 -= dx;
                    g->y1 -= dx;
                    g->y2 -= dx;
                    g->y3 -= dx;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    if (func_80041E7C(e->field_20.word,
                                      g->x0 + (s16)e->field_18,
                                      g->y0 + (s16)e->field_1A,
                                      (struct ProjectionOut *)h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                func_80042188(v, (u8 *)g, (s32)tb[e->ot_index],
                              e->field_14 | bit, h);

                if (((u8 *)e)[0x72] != 0) {
                    x0 = *(s32 *)&e->field_58;
                    x1 = *(s32 *)&e->field_64;
                    *(s32 *)&g->x0 = x0;
                    *(s32 *)&g->x1 = x1;
                    *(s32 *)&g->r0 = *(s32 *)&e->field_5C;
                    *(s32 *)&g->r1 = *(s32 *)&e->field_68;
                    *(u16 *)&g->u0 = *(u16 *)&e->field_60;
                    x2 = *(u16 *)&e->field_6C;
                    setlen(g, twelve);
                    setcode(g, hi);
                    *(u16 *)&g->u1 = x2;
                    func_80042188(v, (u8 *)g, (s32)tb[e->ot_index],
                                  e->field_14 | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}
