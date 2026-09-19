/* The explicit viewport .data view keeps both offsets absolute under the
 * uniform -G8 compiler/assembler profile used by this renderer. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "display_flat_lights.h"
#include "display_object_projection.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_list_renderer_table.h"
#include "display_object_stream_state.h"
#include "display_object_render_frame.h"
#define DISPLAY_OBJECT_PACKET_SUBMIT_RAW_ATTRIBUTE
#include "display_object_packet_submit.h"

#include "ordering_tables.h"

#define POLY_G4_BYTES(packet) ((u8 *)(packet))
#define POLY_GT4_BYTES(packet) ((u8 *)(packet))
#define DISPLAY_OBJECT_BYTES(object) ((u8 *)(object))

/* Both renderers build their packet in the scratchpad at 0x1F800344, and the
 * packet is a libgpu primitive: the two bytes each one writes into the tag
 * word are setlen and setcode, 8 and 0x38 here, which is setPolyG4, and 12
 * and 0x3C in DisplayObject_RenderTexturedGouraudQuadList, which is setPolyGT4. The offsets agree member by
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
 * DisplayObject_SubmitPacket. */
void DisplayObject_RenderGouraudQuadList(void) {
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
                fn(DISPLAY_OBJECT_BYTES(e));
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

                DisplayObject_SubmitPacket(v, POLY_G4_BYTES(g), (s32)tb[e->ot_index],
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
                    DisplayObject_SubmitPacket(v, POLY_G4_BYTES(g), (s32)tb[e->ot_index],
                                               e->field_14 | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}

/* The POLY_GT4 sibling for the list rooted at D_800EFE38[5]: the same walk,
 * with each vertex carrying a texture coordinate as well. Its second
 * submission, gated on the byte at 0x72 two bytes past the record, rewrites
 * the first two vertices from 0x58; display_object.h records why that reach
 * stays a byte reach. */
void DisplayObject_RenderTexturedGouraudQuadList(void) {
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
                fn(DISPLAY_OBJECT_BYTES(e));
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

                DisplayObject_SubmitPacket(v, POLY_GT4_BYTES(g), (s32)tb[e->ot_index],
                                           e->field_14 | bit, h);

                if (DISPLAY_OBJECT_BYTES(e)[0x72] != 0) {
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
                    DisplayObject_SubmitPacket(v, POLY_GT4_BYTES(g), (s32)tb[e->ot_index],
                                               e->field_14 | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}
void DisplayObject_RenderFrame(void)
{
    s32 i;
    SetBackColor(96, 96, 96);
    SetFarColor(0, 0, 0);
    GsSetFlatLight(0, &D_80090FCC);
    GsSetFlatLight(1, &D_80090FDC);
    GsSetFlatLight(2, &D_80090FDC);
    for (i = DISPLAY_OBJECT_LIST_COUNT - 1; i >= 0; i--) {
        if (D_800EFE38[i] >= 0) {
            SetGeomScreen(150);
            SetGeomOffset(0, 0);
            gDisplayObject_ListRenderers[i]();
        }
    }
}

/* The seven display-object stream opcode handlers, entries 0 through 6 of
   D_80090FEC in model_record_tables.c, which DisplayObjectStream_ReadNextCommand dispatches as
   table[op ^ 0xFF] for opcodes 0xFF down to 0xF9: the stream stop, counter
   reset, no-op, two jumps, random jump, and four-operand attribute command.
   Each reads its operands from the object's stream and returns whether the
   interpreter should keep going.

   The three former sources were recorded at gcc_2_8_1_g8 and
   gcc_2_8_1_g8_split, and every member compiles to an identical object at
   gcc_2_8_1_g8_split. Together with the contiguous frame renderer above,
   this is the complete matching run between unmatched display-object code. */

s32 DisplayObjectStream_Stop(DisplayObjectStreamState *object, const u8 *data)
{
    object->field_5A = 0;
    return -1;
}

s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_58 = 0;
    return 1;
}

s32 DisplayObjectStream_Noop(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    return 1;
}

s32 DisplayObjectStream_JumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 DisplayObjectStream_ToggleFlagAndJumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->flags ^= 0x800000;
    object->field_58 = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 DisplayObjectStream_JumpToRandomOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    int i = rand() % data[0];
    unsigned hi, lo;
    u8 *base;

    data += i * 2 + 1;
    hi = data[1];
    lo = data[0];
    base = object->base;
    object->field_58 = 0;
    object->current = base + ((hi << 8) | lo);
    return 1;
}

s32 DisplayObjectStream_ConfigureRotation(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    int high;
    int low;
    object->flags |= GsROTOFF;
    object->field_22 = data[0];
    object->field_4A = (signed char)data[1];
    high = data[3] << 8;
    low = data[2];
    object->current += 4;
    object->field_48 = high | low;
    return 1;
}
