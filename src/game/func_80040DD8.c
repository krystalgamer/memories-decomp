#define GGRAPHICS_VIEWPORT_IN_DATA
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "graphics_frame.h"
#include "display_object_projection.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_list_renderers.h"
#include "../unmatched.h"

#include "ordering_tables.h"

/* Both renderers build their packet in the scratchpad at 0x1F800344, and the
 * packet is a libgpu primitive: the two bytes each one writes into the tag
 * word are setlen and setcode, 8 and 0x38 here, which is setPolyG4, and 12
 * and 0x3C in func_80041068, which is setPolyGT4. The offsets agree member by
 * member -- each x/y pair lands on xN/yN, each colour word on rN, and in the
 * textured form each halfword on uN -- so the cursor is typed as the
 * primitive. The constants stay in `eight`/`hi` rather than setPolyG4's
 * literals because retail holds them in s8/s7 across the loop.
 *
 * Reads from the object that follow a store into the primitive are spelled
 * *(T *)&e->member, not e->member. A member read is a struct reference, the
 * word stores into the primitive are not, and GCC 2.8.1 lets a load at a
 * varying struct address float above a store to a fixed scalar one; taken
 * through the member's address the read stays scalar and keeps its place,
 * which is what retail does. The loads at the top of each vertex block come
 * before any such store and are ordinary member reads. See display_object.h
 * for the measurement.
 *
 * Walks the display-object list from the head index at D_800EFE38[4], calls
 * each object's callback, and for every visible object copies its four
 * vertices into the POLY_G4, offsets them by the viewport origin unless bit 3
 * is set, runs the bit-2 clip test through func_80041E7C, and submits the
 * quad -- twice when the +0x5A flag asks for the second texture -- through
 * func_80042188. */
void func_80040DD8(void) {
    POLY_G4 *g;
    u8 *h;
    DisplayObject *e;
    GsOT **tb;
    DisplayObjectCallback fn;
    s32 eight;
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

    g = (POLY_G4 *)0x1F800344;
    h = (u8 *)0x1F800398;
    i = D_800EFE38[4];

    if (i >= 0) {
        eight = 8;
        hi = 0x38;
        tb = D_800E9D90;
        bit = 0x40000;

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
                w1 = e->field_30.word;
                w2 = e->field_38.word;
                w3 = e->field_40.word;
                *(s32 *)&g->x0 = w0;
                *(s32 *)&g->x1 = w1;
                *(s32 *)&g->x2 = w2;
                *(s32 *)&g->x3 = w3;
                *(s32 *)&g->r0 = *(s32 *)&e->field_2C;
                *(s32 *)&g->r1 = *(s32 *)&e->field_34;
                *(s32 *)&g->r2 = *(s32 *)&e->field_3C;
                w4 = *(s32 *)&e->field_44;
                setlen(g, eight);
                setcode(g, hi);
                fl = e->flags;
                *(s32 *)&g->r3 = w4;

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

                func_80042188((SpritePrim *)v, (u8 *)g, (s32)tb[e->ot_index],
                              e->field_14 | bit, h);

                if (*(u8 *)&e->field_5A != 0) {
                    x0 = e->field_48.word;
                    x1 = e->field_50.word;
                    *(s32 *)&g->x0 = x0;
                    *(s32 *)&g->x1 = x1;
                    /* field_4C is declared s32, so *(s32 *)& would fold
                       back into a member read and float; u32 keeps it
                       scalar. */
                    *(s32 *)&g->r0 = *(u32 *)&e->field_4C;
                    x2 = *(s32 *)&e->field_54;
                    setlen(g, eight);
                    setcode(g, hi);
                    *(s32 *)&g->r1 = x2;
                    func_80042188((SpritePrim *)v, (u8 *)g, (s32)tb[e->ot_index],
                                  e->field_14 | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}
