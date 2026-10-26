#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
/* The explicit viewport .data view keeps both offsets absolute under the
 * uniform -G8 compiler/assembler profile used by these renderers. */
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "display_flat_lights.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "display_object_list_renderer_table.h"
#include "display_object_stream_state.h"
#include "display_object_render_frame.h"
#include "sprite_primitive.h"
#define DISPLAY_OBJECT_PACKET_SUBMIT_ATTRIBUTE_VIEW
#include "display_object_packet_submit.h"
#include "display_object_runtime.h"
#include "ordering_tables.h"

#define DISPLAY_OBJECT_CALLBACK_BYTES(object) ((u8 *)(object))
#define POLY_G4_BYTES(packet) ((u8 *)(packet))
#define POLY_GT4_BYTES(packet) ((u8 *)(packet))
#define DISPLAY_OBJECT_BYTES(object) ((u8 *)(object))
/* Caching this pointer shortens retail; each use must reload field_4C. */
#define SPRITE_SHEET_HEADER(object) \
    ((SpriteSheetHeader *)(object)->field_4C)
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))
#define DISPLAY_OBJECT_COLOR_BYTES(object) ((u8 *)&(object)->rgb)
#ifndef SPRITE_SHEET_SCREEN_HEIGHT
#define SPRITE_SHEET_SCREEN_HEIGHT 240
#endif

/* Submits one display object as a sprite in vertical strips of up to 64
   pixels. Fills the sprite primitive in the scratchpad at 0x1F800320 from
   the object, offsets it by the viewport origin unless bit 3 is set, runs
   the bit-2 clip test through DisplayObject_ProjectRotationFacing (setting the
   POLY_FT4 at 0x1F800344 up as a 9-word packet, semi-transparent when the tag
   says so),
   then walks the object's width through DisplayObject_SubmitPacket, advancing the
   position or the size and the texture coordinates (the page step depends on
   the depth bits of the tag) after each strip. */
void DisplayObject_RenderSpriteStrips(DisplayObject *e, GsOT *ordering_table, s32 mode_arg) {
    u16 mode16 = mode_arg;
    SpritePrim *p;
    ClipState *c;
    POLY_FT4 *g;
    s32 step;
    s32 mode;
    s32 remaining;
    s32 tag;
    s32 fl;

    c = (ClipState *)0x1F800378;
    p = (SpritePrim *)0x1F800320;
    g = (POLY_FT4 *)0x1F800344;
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
    p->rgb = e->rgb;
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
        gDisplayObject_fRenderRetry = 0;
        if (DisplayObject_ProjectRotationFacing(
                e, (s16)p->xy.h.x + (s16)e->field_18,
                (s16)p->xy.h.y + (s16)e->field_1A,
                          (struct ProjectionOut *)c->out) <= 0) {
            return;
        }
        setlen(g, 9);
        *(s32 *)&g->r0 = p->rgb;
        g->code = 0x2C;
        if ((p->attribute & GsALON) != 0) {
            SetSemiTrans(g, 1);
        }
        c->flag = GsROTOFF;
        mode = mode16 | 0xF0000;
    } else if ((p->attribute & GsROTOFF) == 0) {
        p->rotate = e->field_20.h.rotation_z * 5760;
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
        DisplayObject_SubmitPacket(p, (u8 *)g, ordering_table, mode, c->out);
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

void DisplayObject_RenderSpriteStripList(void)
{
    s32 i = gDisplayObject_awListHeads[3];

    if (i >= 0) {
        DisplayObject *base = gDisplayObject_aPool;
        GsOT *G32 *table = gGraphics_apOrderingTables;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = DISPLAY_OBJECT_CALLBACK_BYTES(object);

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                DisplayObject_RenderSpriteStrips(object, table[object->ot_index],
                              (s16)object->ot_depth);
            }
        } while (i >= 0);
    }
}

void DisplayObject_RunUpdateCallbackList(void)
{
    s32 i = gDisplayObject_awListHeads[0];

    while (i >= 0) {
        DisplayObject *object = &gDisplayObject_aPool[i];
        DisplayObjectCallback callback = object->update;

        i = object->next;
        if (callback != 0) {
            callback(DISPLAY_OBJECT_CALLBACK_BYTES(object));
        }
    }
}

void DisplayObject_RunSecondaryCallbackList(void)
{
    s32 i = gDisplayObject_awListHeads[6];

    if (i >= 0) {
        DisplayObject *base = gDisplayObject_aPool;
        GsOT *G32 *table = gGraphics_apOrderingTables;

        do {
            DisplayObject *object =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback callback = object->update;
            u8 *data = DISPLAY_OBJECT_CALLBACK_BYTES(object);

            i = object->next;

            if (callback != 0) {
                callback(data);
            }

            if (((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                void (*secondary)(u8 *, s32) =
                    (void (*)(u8 *, s32))object->field_4C;

                if (secondary != 0) {
                    secondary(data, (s32)table[object->ot_index]);
                }
            }
        } while (i >= 0);
    }
}

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
 * Walks the display-object list from the head index at gDisplayObject_awListHeads[4], calls
 * each object's callback, and for every visible object copies its four
 * vertices into the POLY_G4, offsets them by the viewport origin unless bit 3
 * is set, runs the bit-2 clip test through
 * DisplayObject_ProjectPackedRotationFacing, and submits the quad -- twice when
 * the +0x5A flag asks for the second texture -- through
 * DisplayObject_SubmitPacket. */
void DisplayObject_RenderGouraudQuadList(void) {
    POLY_G4 *g;
    u8 *h;
    DisplayObject *e;
    GsOT *G32 *tb;
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
    i = gDisplayObject_awListHeads[4];

    if (i >= 0) {
        eight = 8;
        hi = 0x38;
        tb = gGraphics_apOrderingTables;
        bit = 0x40000;

        do {
            e = &gDisplayObject_aPool[i];
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
                    if (DisplayObject_ProjectPackedRotationFacing(
                                      e->field_20.word,
                                      g->x0 + (s16)e->field_18,
                                      g->y0 + (s16)e->field_1A,
                                      (struct ProjectionOut *)h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                DisplayObject_SubmitAttributePacket(v, POLY_G4_BYTES(g), tb[e->ot_index],
                                           e->ot_depth | bit, h);

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
                    DisplayObject_SubmitAttributePacket(v, POLY_G4_BYTES(g), tb[e->ot_index],
                                               e->ot_depth | bit, h);
                }
            }
        next:
            ;
        } while (i >= 0);
    }
}

/* The POLY_GT4 sibling for the list rooted at gDisplayObject_awListHeads[5]: the same walk,
 * with each vertex carrying a texture coordinate as well. Its second
 * submission, gated on the byte at 0x72 two bytes past the record, rewrites
 * the first two vertices from 0x58; display_object.h records why that reach
 * stays a byte reach. */
void DisplayObject_RenderTexturedGouraudQuadList(void) {
    POLY_GT4 *g;
    u8 *h;
    DisplayObject *e;
    GsOT *G32 *tb;
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
    i = gDisplayObject_awListHeads[5];

    if (i >= 0) {
        twelve = 0xC;
        hi = 0x3C;
        tb = gGraphics_apOrderingTables;
        bit = 0x50000;

        do {
            e = &gDisplayObject_aPool[i];
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
                    if (DisplayObject_ProjectPackedRotationFacing(
                                      e->field_20.word,
                                      g->x0 + (s16)e->field_18,
                                      g->y0 + (s16)e->field_1A,
                                      (struct ProjectionOut *)h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                DisplayObject_SubmitAttributePacket(v, POLY_GT4_BYTES(g), tb[e->ot_index],
                                           e->ot_depth | bit, h);

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
                    DisplayObject_SubmitAttributePacket(v, POLY_GT4_BYTES(g), tb[e->ot_index],
                                               e->ot_depth | bit, h);
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
    GsSetFlatLight(0, &gDisplayObject_FlatLightDown);
    GsSetFlatLight(1, &gDisplayObject_FlatLightUp);
    GsSetFlatLight(2, &gDisplayObject_FlatLightUp);
    for (i = DISPLAY_OBJECT_LIST_COUNT - 1; i >= 0; i--) {
        if (gDisplayObject_awListHeads[i] >= 0) {
            SetGeomScreen(150);
            SetGeomOffset(0, 0);
            gDisplayObject_ListRenderers[i]();
        }
    }
}

/* Entries 0 through 4 of tent_DisplayObjectStreamCommandTable. */
s32 DisplayObjectStream_Stop(DisplayObjectStreamState *object, const u8 *data)
{
    object->frame_delay = 0;
    return -1;
}

s32 DisplayObjectStream_ResetOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->command_offset = 0;
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
    object->command_offset = 0;
    object->current = object->base + ((data[1] << 8) | data[0]);
    return 1;
}

s32 DisplayObjectStream_ToggleFlagAndJumpToOffset(
    DisplayObjectStreamState *object,
    const u8 *data
)
{
    object->attribute ^= 0x800000;
    object->command_offset = 0;
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
    object->command_offset = 0;
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
    object->attribute |= GsROTOFF;
    object->rotation_z = data[0];
    object->pivot_y = (signed char)data[1];
    high = data[3] << 8;
    low = data[2];
    object->current += 4;
    object->pivot_x = high | low;
    return 1;
}

/* Emits one sprite per sheet part. The object's position is offset by the
 * viewport unless it is screen-space (flag 8). With the clip-test flag (4)
 * the object is projected once through DisplayObject_ProjectRotationFacing,
 * which may ask (gDisplayObject_fRenderRetry) for the setup to run again, and every part is
 * submitted as a
 * 9-word semi-transparent quad at 0x1F800344. Otherwise each part either
 * goes out as a plain sprite, culled against the regional screen bounds when the
 * attribute's 0x08000000 bit asked for that, or as a rotated/scaled sprite
 * about the object's +0x48 pivot. The scale and colour are written through
 * GsSPRITE's own members, which is the view libgs gives the record. */
void DisplayObject_RenderSpriteSheet(DisplayObject *object, GsOT *ordering_table, s32 depth)
{
    SpritePrim *sprite;
    SpriteSheetWork *work;
    SpriteSheetState *state;
    u8 *quad;
    u8 *sheet;
    SpriteSheetPart *part;
    s32 count;
    s32 x;
    s32 y;
    s32 dx;
    s32 dy;
    s32 value;
    s32 tpage;
    u16 cx;

    state = (SpriteSheetState *)0x1F8003A4;
    x = (s16)object->field_30.h.field_30;
    y = (s16)object->field_30.h.field_32;
    if ((object->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        x -= gGraphics_sViewportX;
        y -= gGraphics_sViewportY;
    }
    work = (SpriteSheetWork *)0x1F800378;
    work->mx = object->field_48.h.field_48;
    work->my = object->field_48.h.field_4A;
    sprite = (SpritePrim *)0x1F800320;
    work->clip = object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST;
retry:
    work->attribute = object->attribute;
    work->u = ((u8 *)&object->field_5E)[0];
    work->v = ((u8 *)&object->field_5E)[1];
    count = SPRITE_SHEET_HEADER(object)->count;
    if (count == 0) {
        return;
    }
    state->flags = SPRITE_SHEET_HEADER(object)->flags;
    if (work->attribute & 0x01000000) {
        work->wide = 1;
        tpage = object->field_66 + SPRITE_SHEET_HEADER(object)->tpage * 2;
    } else {
        work->wide = 0;
        tpage = object->field_66 + SPRITE_SHEET_HEADER(object)->tpage;
    }
    sprite->tpage = work->tpage = tpage;
    work->cx = object->field_40.h.field_40;
    work->cy = object->field_40.h.field_42;
    if (object->flags & DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET) {
        work->cx += (SPRITE_SHEET_HEADER(object)->clut & 0xF) << 4;
        work->cy += SPRITE_SHEET_HEADER(object)->clut >> 4;
    }
    sheet = (u8 *)object->field_4C;
    sprite->rotate = object->field_20.h.rotation_z * 5760;
    GS_SPRITE_VIEW(sprite)->scalex = object->field_44.h.field_44;
    GS_SPRITE_VIEW(sprite)->scaley = object->field_44.h.field_46;
    sprite->mxmy.h.y = 0;
    sprite->mxmy.h.x = 0;
    GS_SPRITE_VIEW(sprite)->r = DISPLAY_OBJECT_COLOR_BYTES(object)[0];
    GS_SPRITE_VIEW(sprite)->g = DISPLAY_OBJECT_COLOR_BYTES(object)[1];
    GS_SPRITE_VIEW(sprite)->b = DISPLAY_OBJECT_COLOR_BYTES(object)[2];
    part = (SpriteSheetPart *)(sheet + sizeof(SpriteSheetHeader));
    sprite->cxcy.h.cx = work->cx;
    sprite->cxcy.h.cy = work->cy;
    state->flip = 0;
    state->mirror = 0;
    state->screen = work->attribute & 0x08000000;
    if (state->screen != 0) {
        work->attribute &= 0xF7FFFFFF;
    }
    if (work->attribute & 0x800000) {
        work->attribute &= 0xF7FFFFFF;
        state->flip = 0x2000;
        state->mirror = 0x2000;
    }
    quad = 0;
    work->mode = 0;
    if (work->clip != 0) {
        quad = (u8 *)0x1F800344;
        *(u32 *)&((POLY_FT4 *)quad)->r0 = sprite->rgb;
        setlen(quad, 9);
        setcode(quad, 0x2C);
        gDisplayObject_fRenderRetry = 0;
        if (DisplayObject_ProjectRotationFacing(
                object, x + (s16)object->field_18, y + (s16)object->field_1A,
                &work->proj) <= 0) {
            return;
        }
        if (gDisplayObject_fRenderRetry != 0) {
            goto retry;
        }
        work->mode = 0xF0000;
    }
    do {
        sprite->attribute = work->attribute;
        work->cell = part->cell;
        work->size = part->size;
        sprite->extent.wh.w.word = ((work->size >> 2) & 0x78) + 8;
        sprite->extent.wh.h = ((work->size >> 6) & 0x78) + 8;
        if (state->flags & 0x10) {
            dx = (u8)part->dx | ((part->cell & 0xC000) >> 6);
            if (dx & 0x200) {
                dx |= ~0x1FF;
            }
            dy = (u8)part->dy | ((part->size & 0xC000) >> 6);
            if (dy & 0x200) {
                dy |= ~0x1FF;
            }
        } else {
            dx = part->dx;
            dy = part->dy;
        }
        if (state->mirror != 0) {
            dx = -(dx + sprite->extent.wh.w.word);
        }
        if (state->flags & 0xE0) {
            state->flip = (work->cell ^ state->mirror) & 0x2000;
            if (work->wide != 0) {
                sprite->tpage = work->tpage + ((part->cell >> 9) & 0xE);
                sprite->cxcy.h.cx = work->cx;
                sprite->cxcy.h.cy = work->cy + (work->size & 0x1F);
            } else {
                cx = work->cx;
#ifdef VERSION_EUROPE
                /* European CLUT X coordinates wrap around the 0x280 base. */
                value = cx - 0x280;
#else
                value = cx & 0xFF;
#endif
                value += (work->size & 0xF) << 4;
                sprite->tpage = work->tpage + ((part->cell >> 10) & 7);
#ifdef VERSION_EUROPE
                sprite->cxcy.h.cx = (value & 0xFF) + 0x280;
#else
                sprite->cxcy.h.cx = (cx & 0x300) | (value & 0xFF);
#endif
                sprite->cxcy.h.cy = work->cy + (value >> 8);
            }
        }
        sprite->xy.h.x = x + dx;
        sprite->xy.h.y = y + dy;
        value = ((work->cell & 0x1F) << 3) + work->u;
        sprite->uv.b.lo = value;
        sprite->uv.b.hi = ((work->cell & 0x3E0) >> 2) + (u8)work->v;
        sprite->tpage += value >> 8;
        /* From here `value` is the DisplayObject_SubmitPacket mode: case 1 sorts a fast
         * sprite, 2 a flipped one, 3 a rotated/scaled one. */
        value = (u16)depth | 0x10000;
        if (state->flip != 0) {
            value = (u16)depth | 0x20000;
            sprite->attribute |= 0x800000;
            if (sprite->uv.b.hi + sprite->extent.wh.h >= 0x100) {
                sprite->extent.wh.h--;
            }
            if (work->wide != 0) {
                if (sprite->uv.b.lo + sprite->extent.wh.w.word >= 0x100 &&
                    (s8)sprite->uv.b.lo < 0 && sprite->tpage != 0x1F) {
                    sprite->tpage++;
                    sprite->uv.b.lo += 0x80;
                    state->flags |= 0x80;
                } else if (sprite->uv.b.lo == 0) {
                    if (sprite->tpage != 0x10) {
                        sprite->tpage--;
                        sprite->uv.b.lo = 0x80;
                        state->flags |= 0x80;
                    } else {
                        goto narrow;
                    }
                }
            } else {
narrow:
                if (sprite->uv.b.lo == 0) {
                    sprite->uv.b.lo++;
                    sprite->extent.wh.w.word--;
                }
            }
        }
        if (work->clip == 0) {
            if (state->screen != 0) {
                if ((s16)sprite->xy.h.x >= 320 ||
                    (s16)sprite->xy.h.x + sprite->extent.wh.w.word <= 0 ||
                    (s16)sprite->xy.h.y >= SPRITE_SHEET_SCREEN_HEIGHT ||
                    (s16)sprite->xy.h.y + sprite->extent.wh.h <= 0) {
                    goto next;
                }
            } else {
                value = (u16)depth | 0x30000;
                sprite->mxmy.h.x = work->mx - dx;
                sprite->mxmy.h.y = work->my - dy;
                sprite->xy.h.x += sprite->mxmy.h.x;
                sprite->xy.h.y += sprite->mxmy.h.y;
            }
        }
        DisplayObject_SubmitPacket(sprite, quad, ordering_table, value | work->mode, (u8 *)&work->proj);
next:
        part++;
    } while (--count != 0);
}
