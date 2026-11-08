
#include "../types.h"

#include "display_object_depth_bases.h"
#include "mem_card_work.h"
#include "mem_card_diagnostic_strings.h"
#include "mem_card_directory.h"
#include "mem_card_directory_patterns.h"

/* The .sdata window at 0x8009AF6C, carved as one unit because its objects
 * tile it exactly with no gaps: 4 + 4 + 8 + 4 + 8 = 28 bytes, which is
 * precisely the distance to model_graphics_state at 0x8009AF88.
 *
 * Memory-card strings, display-depth defaults and a MIDI marker share this
 * window, so this file is named for its address rather than one subsystem,
 * the way data_80091510.c is.
 * Splitting it by owner is not possible: the units are smaller than the
 * granularity the split file works in.
 *
 * Splat names seven labels in this range, not five. gDisplayObject_awOrderingTableDepthBases below is one
 * eight-byte object, but code addresses its middle elements directly, so the
 * disassembler also named 0x8009AF76 and 0x8009AF7A. Those two labels are
 * positions inside an array, not separate objects, and defining them
 * separately here would be wrong even though it would produce identical
 * bytes. display_object_helpers.h reached that conclusion first and records
 * the reasoning; this file follows it.
 *
 * Every object carries an explicit .sdata attribute, as the neighbouring
 * carves do. Nothing here is entirely zero, so none of it is at risk of
 * migrating to .sbss, but the attribute is kept for consistency with
 * model_background.c and save_data_payload.c and so that the
 * placement is stated rather than inferred from the -G8 threshold.
 *
 * Array sizes are written out rather than left to the initializer. Each one
 * is what the retail layout requires. The two wildcard arrays include
 * padding beyond their terminators; unsized initializers would omit it. */

/* Directory-entry diagnostic in MemCardWork_UpdateOperation. */
/* The European builds have no directory-entry format here. */
#ifndef VERSION_EUROPE
char gMemCard_szDirectoryEntryFormat[4] = "%s\n";
#endif

/* Read by MemCardDialog_UpdateSave (mem_card_dialog.c). */
u8 gMemCardDialog_szAllFilesPattern[4] = "*";

/* Indexed by a display object's ot_index. Declared in
 * display_object_helpers.h, which explains the volatile qualifier and why
 * elements [1] and [3] used to carry private names of their own. The
 * qualifier is load-bearing in display_object_helpers.c, so it belongs on
 * the definition too. */
volatile u16 gDisplayObject_awOrderingTableDepthBases[4] = {
    0x0000,
    0x0020,
    0x0800,
    0x0020,
};

/* Read by MemCard_DoLoadDirectory as the pattern that lists every file. Same
 * single character as gMemCardDialog_szAllFilesPattern, but a separate object in the image, so it
 * stays separate here. */
u8 gMemCard_szAllFilesPattern[4] = "*";

/* The MIDI track chunk tag. SD_FindMidiTrackChunk compares against it
 * while walking a sequence, which is what the tag means in a standard MIDI
 * file: the four bytes that introduce each track. */
u8 gSD_szMidiTrackMarker[8] = "MTrk";

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_config.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "display_object_render_sprite_list.h"
#include "graphics_frame.h"
#include "ordering_tables.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"
#include "view_state.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * display_object_system_commons/_bss_commons subsegments place it at the
 * retail addresses. */
u16 gDisplayObject_wUnlinkCount;
u16 gDisplayObject_wAcquireCount;
s32 gDisplayObject_fRenderRetry;
s16 gDisplayObject_awListHeads[DISPLAY_OBJECT_LIST_COUNT];
DisplayObject gDisplayObject_aPool[DISPLAY_OBJECT_POOL_CAPACITY];
ViewState gViewState_Current;
s16 gDisplayObject_awListKeys[DISPLAY_OBJECT_LIST_COUNT];

#ifndef DISPLAY_OBJECT_SPRITE_TEXTURE_U
#define DISPLAY_OBJECT_SPRITE_TEXTURE_U(texture) ((texture) & 0x3F0)
#endif

#ifndef DISPLAY_OBJECT_SPRITE_TEXTURE_V
#define DISPLAY_OBJECT_SPRITE_TEXTURE_V(texture) (((texture) & 0xF) + 0xF0)
#endif

s32 DisplayObject_FindFreeGeneralSlot(void)
{
    DisplayObject *entry = &gDisplayObject_aPool[DISPLAY_OBJECT_RESERVED_CAPACITY];
    s32 i;

    for (i = DISPLAY_OBJECT_RESERVED_CAPACITY;
         i < DISPLAY_OBJECT_POOL_CAPACITY;
         i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

s32 DisplayObject_FindFreeSlot(void)
{
    DisplayObject *entry = gDisplayObject_aPool;
    s32 i;

    for (i = 0; i < DISPLAY_OBJECT_POOL_CAPACITY; i++, entry++) {
        if ((entry->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
            return i;
        }
    }
    return -1;
}

void *DisplayObject_AcquireSlot(s32 index, s32 key)
{
    DisplayObject *slot;
    s16 other;
    u16 initialized;

    if (index < 0) {
        return 0;
    }

    slot = &gDisplayObject_aPool[index];
    initialized = slot->flags & DISPLAY_OBJECT_FLAG_ALLOCATED;
    gDisplayObject_wAcquireCount++;
    if (!initialized) {
        other = gDisplayObject_awListHeads[key];
        if (other < 0) {
            gDisplayObject_awListKeys[key] = index;
            slot->next = -1;
            slot->previous = -1;
        } else {
            gDisplayObject_aPool[other].previous = index;
            slot->next = (u16)gDisplayObject_awListHeads[key];
        }

        slot->previous = -1;
        gDisplayObject_awListHeads[key] = index;
        /* A fresh object starts with rotation off. */
        slot->attribute = GsROTOFF;
        slot->flags = DISPLAY_OBJECT_RENDERABLE_MASK;
        slot->ot_index = 2;
        slot->field_54 = tail_data_start;
        slot->field_6C = 0;
        slot->update = 0;
        slot->list_key = key;
        slot->pool_index = index;
        slot->field_0B = 0;
        slot->rgb = 0x00808080;
        slot->field_20.word = 0;
        slot->back_face_handler = 0;
        slot->field_1C = 0;
        slot->field_18.center.y = 0;
        slot->field_18.center.x = 0;
        slot->texture_page = 0;
        slot->field_48.word = 0;
        slot->field_44.word = 0x10001000;
        slot->field_40.word = 0;
        slot->resource_uv = 0;
        slot->subdivisions = 0;
        slot->depth_offset = 0;
        slot->ot_depth = gDisplayObject_awOrderingTableDepthBases[slot->ot_index];
    }
    return slot;
}

void DisplayObject_Release(DisplayObject *slot)
{
    s32 first = slot->previous;
    s32 second = slot->next;

    gDisplayObject_wUnlinkCount++;

    if (first < 0) {
        s16 index = slot->list_key;
        gDisplayObject_awListHeads[index] = second;
        if (second >= 0) {
            gDisplayObject_aPool[second].previous = -1;
        }
    } else {
        gDisplayObject_aPool[first].next = second;
        if (second >= 0) {
            gDisplayObject_aPool[second].previous = first;
        }
    }

    slot->flags = 0;
}

void DisplayObject_MoveToListHead(DisplayObject *slot, s32 key)
{
    u16 saved = slot->flags;
    s32 v;

    DisplayObject_Release(slot);
    v = gDisplayObject_awListHeads[key];
    if (v < 0) {
        gDisplayObject_awListKeys[key] = slot->pool_index;
        slot->next = -1;
        slot->previous = -1;
    } else {
        gDisplayObject_aPool[v].previous = slot->pool_index;
        slot->next = gDisplayObject_awListHeads[key];
    }
    slot->previous = -1;
    gDisplayObject_awListHeads[key] = slot->pool_index;
    slot->flags = saved;
}

void DisplayObject_ReleaseIfPresent(void *object)
{
    if (object != 0) {
        DisplayObject_Release((DisplayObject *)object);
    }
}

void DisplayObject_ResetPool(void){int i=0;int neg=-1;s16*a; s16*b;a=gDisplayObject_awListKeys;b=gDisplayObject_awListHeads;gDisplayObject_wUnlinkCount=0;gDisplayObject_wAcquireCount=0;for(;i<DISPLAY_OBJECT_LIST_COUNT;i++){*b=neg;*a=neg;a++;b++;}{DisplayObject*p=gDisplayObject_aPool;for(i=DISPLAY_OBJECT_POOL_CAPACITY-1;i>=0;i--){p->flags=0;p++;}}}

void DisplayObject_Reset(void)
{
    DisplayObject_ResetPool();
}

void DisplayObject_SetResourceVariant(DisplayObjectConfig *object, s32 value)
{
    object->resource_variant = value;
    object->flags &= 0xFFEF;
}

void DisplayObject_UpdateResourceVariant(
    DisplayObjectConfig *object,
    s32 value
)
{
    if (object->resource_variant != value) {
        object->resource_variant = value;
        object->flags &= 0xFFEF;
    }
}

void DisplayObject_SetResourcePath(
    DisplayObjectConfig *object,
    u8 resource_root_index,
    u8 resource_branch_index,
    u8 resource_variant
)
{
    object->resource_root_index = resource_root_index;
    object->resource_branch_index = resource_branch_index;
    object->resource_variant = resource_variant;
    object->flags &= 0xFFEF;
}

void *DisplayObject_ConfigureSpriteResource(DisplayObject *configured, int resource_root_index, int resource_branch_index,
                    int resource_variant, int texture_config, int palette_config)
{
    u16 flags;
#ifdef DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE
    int texture_v;
#endif

    configured->field_67 = resource_root_index;
    configured->field_68 = resource_branch_index;
    configured->field_69 = resource_variant;
    configured->texture_page = texture_config;
    *(u8 *)&configured->resource_uv = texture_config >> 16;
    *((u8 *)&configured->resource_uv + 1) = texture_config >> 8;
    configured->field_40.clut.x = DISPLAY_OBJECT_SPRITE_TEXTURE_U(palette_config);
#ifdef DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE
    texture_v = DISPLAY_OBJECT_SPRITE_TEXTURE_V_BASE(palette_config);
    configured->field_40.clut.y = texture_v + (palette_config & 0xF);
#else
    configured->field_40.clut.y = DISPLAY_OBJECT_SPRITE_TEXTURE_V(palette_config);
#endif
    flags = configured->flags & ~DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    configured->flags = flags;
    if (palette_config & 0x8000) {
        configured->flags = flags | DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET;
    }
    return configured;
}

void DisplayObject_ConfigureSpriteAtPosition(void *object, s32 x, s32 y, s32 resource_root_index, s32 resource_branch_index,
                   s32 resource_variant, s32 texture_config, s32 palette_config)
{
    DisplayObject *o = object;

    o->xy.h.x = x;
    o->xy.h.y = y;
    DisplayObject_ConfigureSpriteResource(object, resource_root_index, resource_branch_index, resource_variant, texture_config, palette_config);
}

DisplayObject *DisplayObject_ConfigureScreenSprite(
    DisplayObject *object,
    s32 x,
    s32 y,
    s32 width,
    s32 height,
    s32 texture_u,
    s32 texture_v,
    s32 texture_page,
    s32 clut_x,
    s32 clut_y)
{
    s32 half_width;
    s32 half_height;

    object->field_3C.size.w = width;
    half_width = width / 2;
    object->xy.h.x = x;
    object->xy.h.y = y;
    object->field_18.center.x = half_width;
    object->field_48.pivot.x = half_width;

    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;

    object->field_3C.size.h = height;
    half_height = height / 2;
    *(u8 *)&object->field_5C = texture_u;
    ((u8 *)&object->field_5C)[1] = texture_v;
    object->texture_page = texture_page;
    object->field_40.clut.x = clut_x;
    object->field_40.clut.y = clut_y;
    object->field_18.center.y = half_height;
    object->field_48.pivot.y = half_height;
    return object;
}

/* Walks the display-object list rooted at gDisplayObject_awListHeads[1]: calls each object's
   callback, and for every renderable object fills the sprite primitive in
   the scratchpad at 0x1F800320 from the object, offsets it by the viewport
   origin unless bit 3 is set, and submits it through DisplayObject_SubmitPacket. With
   bit 2 the position first goes through the clip test in
   DisplayObject_ProjectRotationFacing, which may ask (gDisplayObject_fRenderRetry) for the
   object to be run again, and the
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

    i = gDisplayObject_awListHeads[1];
    if (i >= 0) {
        g = (POLY_FT4 *)0x1F800344;
        p = (SpritePrim *)0x1F800320;
        h = (ClipState *)0x1F800378;
        tb = gGraphics_apOrderingTables;

        do {
            e = (DisplayObject *)((u8 *)gDisplayObject_aPool + i * DISPLAY_OBJECT_RECORD_SIZE);
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
                p->rgb = e->rgb;
                p->xy.word = e->xy.word;
                p->extent.word = e->field_3C.word;
                p->cxcy.word = e->field_40.word;
                p->uv.word = e->field_5C;
                mode = e->ot_depth | 0x10000;
                ot = tb[idx];
                p->tpage = e->texture_page;

                if ((e->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
                    p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
                    p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
                }

                if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
                    gDisplayObject_fRenderRetry = 0;
                    if (DisplayObject_ProjectRotationFacing(
                            e, (s16)p->xy.h.x + (s16)e->field_18.center.x,
                            (s16)p->xy.h.y + (s16)e->field_18.center.y,
                                      (struct ProjectionOut *)h->out) <= 0) {
                        break;
                    }
                    if (gDisplayObject_fRenderRetry != 0) {
                        continue;
                    }
                    setlen(g, 9);
                    *(s32 *)&g->r0 = p->rgb;
                    g->code = 0x2C;
                    if ((p->attribute & GsALON) != 0) {
                        SetSemiTrans(g, 1);
                    }
                    mode = e->ot_depth | 0xF0000;
                } else if ((p->attribute & GsROTOFF) == 0) {
                    p->rotate = e->field_20.h.rotation_z * 5760;
                    p->scale = e->field_44.word;
                    p->mxmy.word = e->field_48.word;
                    p->xy.h.x = p->xy.h.x + p->mxmy.h.x;
                    p->xy.h.y = p->xy.h.y + p->mxmy.h.y;
                    mode = e->ot_depth | 0x30000;
                }
                DisplayObject_SubmitPacket(p, (u8 *)g, ot, mode, h->out);
                break;
            }
        } while (i >= 0);
    }
}

#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_update_command_stream.h"
#include "../unmatched.h"
#include "display_object_runtime.h"
#include "display_object_render_sprite_sheet_list.h"

#include "ordering_tables.h"

void DisplayObject_RenderSpriteSheetList(void) {
    s32 i = gDisplayObject_awListHeads[2];

    if (i >= 0) {
        DisplayObject *base = gDisplayObject_aPool;
        GsOT **t = gGraphics_apOrderingTables;

        do {
            DisplayObject *p =
                (DisplayObject *)(i * DISPLAY_OBJECT_RECORD_SIZE + (s32)base);
            DisplayObjectCallback f = p->update;
            u8 *q = (u8 *)p;

            i = p->next;

            if (f != 0) {
                f(q);
            }

            if (((p->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ^
                 DISPLAY_OBJECT_RENDERABLE_MASK) == 0) {
                DisplayObject_UpdateCommandStream((DisplayObject *)q);
                DisplayObject_RenderSpriteSheet(
                    (DisplayObject *)q, t[p->ot_index],
                    (s16)p->ot_depth
                );
            }
        } while (i >= 0);
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/rand.h"
/* The explicit viewport .data view keeps both offsets absolute under the
 * uniform -G8 compiler/assembler profile used by these renderers. */
#include "graphics_frame.h"
#include "display_flat_lights.h"
#include "display_object.h"
#include "display_object_layout.h"
#include "display_object_projection.h"
#include "display_object_list_renderer_table.h"
#include "display_object_stream_state.h"
#include "display_object_render_frame.h"
#include "sprite_primitive.h"
#include "display_object_packet_submit.h"
#include "display_object_runtime.h"
#include "ordering_tables.h"
#include "../unmatched.h"
#include "display_object_render_sprite_list.h"
#include "display_object_render_sprite_sheet_list.h"

/* Initialized data this unit owns at 0x80090FB0: the per-list display object renderers.
 *
 * This is the one table in this group with a real bound. DisplayObject_RenderFrame walks
 * it with `for (i = 0; i < DISPLAY_OBJECT_LIST_COUNT; i++)`, that constant is
 * seven in display_object_layout.h, and there are seven entries. It agrees
 * independently with display_object.h, where the list heads are
 * `s16 gDisplayObject_awListHeads[DISPLAY_OBJECT_LIST_COUNT]`, and with
 * DisplayObject_ResetPool, which clears the same count.
 *
 * So this array is declared sized, unlike the other three, because the size
 * is established rather than assumed. */

void (*gDisplayObject_ListRenderers[DISPLAY_OBJECT_LIST_COUNT])(void) = {
    DisplayObject_RunUpdateCallbackList,
    DisplayObject_RenderSpriteList,
    DisplayObject_RenderSpriteSheetList,
    DisplayObject_RenderSpriteStripList,
    DisplayObject_RenderGouraudQuadList,
    DisplayObject_RenderTexturedGouraudQuadList,
    DisplayObject_RunSecondaryCallbackList,
};

/* Initialized data this unit owns at 0x80090FCC: the two flat lights
 * DisplayObject_RenderFrame installs before walking the display object lists.
 *
 * The type is not inferred. DisplayObject_RenderFrame passes both to
 * GsSetFlatLight, whose second parameter is a GsF_LIGHT *, so the records are
 * that type by the call rather than by resemblance. Sixteen bytes each, which
 * is what GsF_LIGHT occupies once its three unsigned chars are padded out.
 *
 * The two are a key and a fill: one direction vector points along +y at
 * intensity 0x60, the other along -y at 0x30, and DisplayObject_RenderFrame
 * installs the second one in both slots 1 and 2.
 *
 * gDisplayObject_FlatLightUp's gap to the next name is 0x2C, which is NOT its size. The
 * twenty-eight bytes above it hold seven pointers into
 * DisplayObject_RenderFrame's own range and carry no name of their own, so
 * they stay in the blob rather than being claimed here. Reading that gap as
 * the size is exactly the failure memory-map.md records. */

GsF_LIGHT gDisplayObject_FlatLightDown = { 0, 0x1000, 0, 0x60, 0x60, 0x60 };
GsF_LIGHT gDisplayObject_FlatLightUp = { 0, -0x1000, 0, 0x30, 0x30, 0x30 };

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
    p->tpage = e->texture_page;
    p->xy.word = e->xy.word;
    p->cxcy.word = e->field_40.word;
    p->rgb = e->rgb;
    fl = e->flags;
    p->uv.word = e->field_5C;
    if ((fl & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        p->xy.h.x = p->xy.h.x - gGraphics_sViewportX;
        p->xy.h.y = p->xy.h.y - gGraphics_sViewportY;
    }
    p->extent.wh.h = e->field_3C.size.h;
    remaining = e->field_3C.size.w;
    c->flag = p->attribute & GsROTOFF;
    mode = mode16 | 0x10000;

    if ((e->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST) != 0) {
        gDisplayObject_fRenderRetry = 0;
        if (DisplayObject_ProjectRotationFacing(
                e, (s16)p->xy.h.x + (s16)e->field_18.center.x,
                (s16)p->xy.h.y + (s16)e->field_18.center.y,
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
        GsOT **table = gGraphics_apOrderingTables;

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
        GsOT **table = gGraphics_apOrderingTables;

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
                w1 = e->xy.word;
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
                                      g->x0 + (s16)e->field_18.center.x,
                                      g->y0 + (s16)e->field_18.center.y,
                                      (struct ProjectionOut *)h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                DisplayObject_SubmitPacket((SpritePrim *)v, POLY_G4_BYTES(g), tb[e->ot_index],
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
                    DisplayObject_SubmitPacket((SpritePrim *)v, POLY_G4_BYTES(g), tb[e->ot_index],
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
                *(u16 *)&g->u0 = *(u16 *)&e->xy;
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
                                      g->x0 + (s16)e->field_18.center.x,
                                      g->y0 + (s16)e->field_18.center.y,
                                      (struct ProjectionOut *)h) <= 0) {
                        goto next;
                    }
                    v = v | 0x4000000;
                }

                DisplayObject_SubmitPacket((SpritePrim *)v, POLY_GT4_BYTES(g), tb[e->ot_index],
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
                    DisplayObject_SubmitPacket((SpritePrim *)v, POLY_GT4_BYTES(g), tb[e->ot_index],
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
    x = (s16)object->xy.h.x;
    y = (s16)object->xy.h.y;
    if ((object->flags & DISPLAY_OBJECT_FLAG_SCREEN_SPACE) == 0) {
        x -= gGraphics_sViewportX;
        y -= gGraphics_sViewportY;
    }
    work = (SpriteSheetWork *)0x1F800378;
    work->mx = object->field_48.pivot.x;
    work->my = object->field_48.pivot.y;
    sprite = (SpritePrim *)0x1F800320;
    work->clip = object->flags & DISPLAY_OBJECT_FLAG_CLIP_TEST;
retry:
    work->attribute = object->attribute;
    work->u = ((u8 *)&object->resource_uv)[0];
    work->v = ((u8 *)&object->resource_uv)[1];
    count = SPRITE_SHEET_HEADER(object)->count;
    if (count == 0) {
        return;
    }
    state->flags = SPRITE_SHEET_HEADER(object)->flags;
    if (work->attribute & 0x01000000) {
        work->wide = 1;
        tpage = object->texture_page + SPRITE_SHEET_HEADER(object)->tpage * 2;
    } else {
        work->wide = 0;
        tpage = object->texture_page + SPRITE_SHEET_HEADER(object)->tpage;
    }
    sprite->tpage = work->tpage = tpage;
    work->cx = object->field_40.clut.x;
    work->cy = object->field_40.clut.y;
    if (object->flags & DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET) {
        work->cx += (SPRITE_SHEET_HEADER(object)->clut & 0xF) << 4;
        work->cy += SPRITE_SHEET_HEADER(object)->clut >> 4;
    }
    sheet = (u8 *)object->field_4C;
    sprite->rotate = object->field_20.h.rotation_z * 5760;
    GS_SPRITE_VIEW(sprite)->scalex = object->field_44.scale.x;
    GS_SPRITE_VIEW(sprite)->scaley = object->field_44.scale.y;
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
                object, x + (s16)object->field_18.center.x, y + (s16)object->field_18.center.y,
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

#include "model_record_tables.h"
#include "display_object_stream_read_next_command.h"
#include "display_object_stream_state.h"

/* Initialized data this unit owns at 0x80090FEC, previously a generated blob
   (#2602).

   tent_DisplayObjectStreamCommandTable is the display-object stream handler table.
   DisplayObjectStream_ReadNextCommand dispatches table[op ^ 0xFF] for opcodes
   from 0xF0 up. All seven callbacks receive the shared DisplayObjectStreamState
   and operand cursor, including the reset and no-op entries that do not need
   both arguments. This keeps the interpreter boundary checked instead of
   hiding five prototype differences behind table casts.

   The background records and shadow fan steps that follow it in this image
   belong to model_background_records.c: Japan links the movie-stream and
   model-effect data between the two. */
s32 (*tent_DisplayObjectStreamCommandTable[])(DisplayObjectStreamState *, const u8 *) = {
    DisplayObjectStream_Stop,
    DisplayObjectStream_ResetOffset,
    DisplayObjectStream_Noop,
    DisplayObjectStream_JumpToOffset,
    DisplayObjectStream_ToggleFlagAndJumpToOffset,
    DisplayObjectStream_JumpToRandomOffset,
    DisplayObjectStream_ConfigureRotation,
};

void DisplayObjectStream_ReadNextCommand(
    DisplayObjectStreamState *object, s32 script, s32 attribute, DisplayObject *owner
)
{
    u8 *p;
    s32 op;
    s32 (**table)(DisplayObjectStreamState *, const u8 *);
    s32 value;

    p = object->current + (u16)object->command_offset;
    op = *p;
    p++;

    if (op >= 0xF0) {
        table = tent_DisplayObjectStreamCommandTable;
        do {
            if (table[op ^ 0xFF](object, p) == -1) {
                return;
            }
            p = object->current + (u16)object->command_offset;
            op = *p;
            p++;
        } while (op >= 0xF0);
    }

    object->frame_delay = op;
    value = (p[1] << 8) | p[0];
    object->frame_data = object->base + value;
    object->command_offset += 3;
}

#include "display_object.h"
#include "display_object_update_command_stream.h"
#include "display_object_stream_read_next_command.h"
#include "graphics_frame.h"
#include "graphics_frame_controls.h"

/* If object's 0x10 state bit isn't set yet: marks it set, resets
   field_58/field_5A, then walks a 3-level little-endian-u16 offset chain
   through the object's field_54 byte buffer (indexed at each level by
   field_67, field_68, field_69) to resolve a final pointer. That pointer and a
   copy of the attribute (masked to clear bit 0x00800000, restored if flags'
   0x100 bit is set) become the args for DisplayObjectStream_ReadNextCommand,
   which always fires on this path. Otherwise (0x10 already set): if neither
   of flags' low two bits are set and the field_5A cooldown is nonzero,
   decrements it by gGraphics_nBaseFrameStep and fires DisplayObjectStream_ReadNextCommand
   once it reaches zero or below.

   script and attribute are locals. The first-time path assigns both before
   jumping to the call, but the cooldown path reaches the same call with
   neither assigned, so there they are whatever $a1 and $a2 still hold, as in
   the retail image.

   The three chain steps use separately-named pointer/offset locals
   (p1/off1, p2/off2, p3/off3) rather than one reused pair -- reusing a
   single pair compiles to the same logic but gcc allocates a spare
   register (a0) for the 2nd/3rd steps instead of matching the ROM's
   plain v0/v1 reuse. */
void DisplayObject_UpdateCommandStream(DisplayObject *object) {
    s32 script;
    s32 attribute;
    u8 *base;
    u8 *p;

    if (!(object->flags & 0x10)) {
        u8 *p1, *p2, *p3;
        s32 off1, off2, off3;
        s32 mask;

        base = (u8 *)object->field_54;
        object->flags = object->flags | 0x10;
        object->field_58 = 0;
        object->field_5A = 1;

        p1 = (u8 *)&((u16 *)base)[object->field_67];
        off1 = p1[1] << 8;
        off1 |= p1[0];
        p2 = (u8 *)&((u16 *)(base + off1))[object->field_68];
        off2 = p2[1] << 8;
        off2 |= p2[0];
        p3 = (u8 *)&((u16 *)(base + off2))[object->field_69];
        off3 = p3[1] << 8;
        off3 |= p3[0];
        p = base + off3;

        mask = object->attribute & 0xFF7FFFFF;
        script = (s32)p;
        attribute = mask;
        object->attribute = mask;
        object->field_50.word = (s32)p;
        if (object->flags & 0x100) {
            object->attribute = mask | 0x800000;
        }
        goto call_it;
    }

    if (!(object->flags & 3) && object->field_5A != 0) {
        s16 remain = (u16)object->field_5A - (u16)gGraphics_nBaseFrameStep;
        object->field_5A = remain;
        if (remain <= 0) {
call_it:
            DisplayObjectStream_ReadNextCommand((DisplayObjectStreamState *)object, script, attribute, object);
        }
    }
}

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "display_object.h"
#include "display_object_projection.h"
#include "model.h"

/* Contiguous projection helpers sharing the scratchpad matrix/vector layout.
 * The first clips packed rotation bytes; the second extends the same path with
 * display-object scale, callback, and mirrored-rotation handling. */

s32 DisplayObject_ProjectPackedRotationFacing(
    u32 packed_rotation, s32 x, s32 y, struct ProjectionOut *out)
{
    SVECTOR *v308;
    MATRIX *mtx;
    s32 clip_value;

    out->x = x;
    out->y = y;
    out->divisions = 0;

    SetGeomOffset(x, y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);

    mtx = (MATRIX *)0x1F8002D0;
    v308 = (SVECTOR *)0x1F800308;

    v308->vx = (s16)((packed_rotation & 0xFF) * 0x10);
    v308->vy = (s16)((packed_rotation >> 4) & 0xFF0);
    v308->vz = (s16)((packed_rotation >> 0xC) & 0xFF0);

    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = MODEL_DEFAULT_PROJECTION;

    GsSetLsMatrix(mtx);

    {
        register SVECTOR *v318 = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;

        RotAverageNclip3_nom(v308, v310, v318);
    }

    {
        s32 *p = &clip_value;
        /* NCLIP leaves the signed projected area in MAC0. */
        gte_stopz(p);
    }

    return clip_value;
}

s32 DisplayObject_ProjectRotationFacing(
    DisplayObject *obj,
    s32 x,
    s32 y,
    struct ProjectionOut *out
)
{
    MATRIX *mtx = (MATRIX *)0x1F8002D0;
    SVECTOR *v308 = (SVECTOR *)0x1F800308;
    s32 facing_area;

    out->x = x;
    out->y = y;
    out->divisions = obj->subdivisions;

    SetGeomOffset(x, y);
    SetGeomScreen(MODEL_DEFAULT_PROJECTION);

    v308->vx = (s16)(obj->field_20.b.rotation_x * 0x10);
    v308->vy = (s16)(obj->field_20.b.rotation_y * 0x10);
    v308->vz = (s16)(obj->field_20.b.rotation_z * 0x10);
    RotMatrixZYX_gte(v308, mtx);

    mtx->t[0] = 0;
    mtx->t[1] = 0;
    mtx->t[2] = MODEL_DEFAULT_PROJECTION;

    if (!(obj->attribute & GsROTOFF)) {
        VECTOR *scale = (VECTOR *)v308;

        scale->vx = obj->field_44.scale.x;
        scale->vy = obj->field_44.scale.y;
        scale->vz = ONE;
        ScaleMatrix(mtx, scale);
    }

    GsSetLsMatrix(mtx);

    {
        SVECTOR *v318 = (SVECTOR *)0x1F800318;
        SVECTOR *v310 = (SVECTOR *)0x1F800310;
        s32 *area_result;

        v318->vx = 0;
        v308->vx = 0;
        v308[1].vx = 0x200;
        v310->vy = 0;
        v308->vy = 0;
        v308[2].vy = 0x200;
        v318->vz = 0;
        v310->vz = 0;
        v308->vz = 0;
        RotAverageNclip3_nom(v308, v310, v318);

        area_result = &facing_area;
        gte_stopz(area_result);

        {
            s32 cb = obj->back_face_handler;
            if (cb != 0) {
                if (cb < 0) {
                    ((void (*)(DisplayObject *, s32))cb)(obj, facing_area);
                }
                if (facing_area >= 0) {
                    return facing_area;
                }

                mtx->m[0][0] = -mtx->m[0][0];
                mtx->m[1][0] = -mtx->m[1][0];
                mtx->m[2][0] = -mtx->m[2][0];
                mtx->m[0][2] = -mtx->m[0][2];
                mtx->m[1][2] = -mtx->m[1][2];
                mtx->m[2][2] = -mtx->m[2][2];
                SetRotMatrix(mtx);

                RotAverageNclip3_nom(
                    v308, v310, (SVECTOR *)0x1F800318);

                area_result = &facing_area;
                gte_stopz(area_result);
            }
        }
    }

    return facing_area;
}

#include "graphics_packet_submission.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "gpu_packets.h"
#include "display_object_config.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "display_object_lifecycle.h"
#include "display_object.h"
#include "display_object_packet_submit.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "trig_constants.h"
#include "display_object_interpolation.h"

/* Regional value: DIVPOLYGON4 piv (272 -> 288). The European build
 * (src/game/european/) defines its own. */
#ifndef DISPLAY_OBJECT_DIVP_PIV
#define DISPLAY_OBJECT_DIVP_PIV 272
#endif

/* Scratchpad work areas: the four quad vertices at 0x1F800300, the
 * RotAverageNclip4 depth/flag results at 0x1F8002E0, and the DivideFT4
 * inputs - its DIVPOLYGON4 at 0x1F800000, the colour at 0x1F800280 and the
 * four texture coordinates from 0x1F800290. */
#define SCRATCH_VERTEX(i) ((SVECTOR *)0x1F800300 + (i))
#define GS_SPRITE_VIEW(sprite) ((GsSPRITE *)(sprite))
#define GS_OT_VIEW(ordering_table) ((GsOT *)(ordering_table))
#define POLY_G4_VIEW(packet) ((POLY_G4 *)(packet))
#define POLY_GT4_VIEW(packet) ((POLY_GT4 *)(packet))
#define POLY_FT4_VIEW(packet) ((POLY_FT4 *)(packet))

/* The high half of `mode` selects the handler and the low half is the
 * ordering-table depth. Cases 1-3 hand a GsSPRITE to libgs. Cases 4 and 5 get
 * the display object's attribute word in place of the sprite pointer and
 * submit a prepared POLY_G4 or POLY_GT4, projecting it first when GsPERS
 * (0x04000000) is set. Any other case builds a POLY_FT4 from the sprite and
 * projects it, subdividing through DivideFT4 when `extra` asks for it. */
void DisplayObject_SubmitPacket(SpritePrim *sprite, u8 *packet, GsOT *ot, s32 mode, u8 *extra)
{
    SVECTOR *v;
    long *otz;
    DisplayObjectPacketOrigin *origin = (DisplayObjectPacketOrigin *)extra;
    s32 pri = (s16)mode;

    switch ((u32)mode >> 16) {
    case 1:
        GsSortFastSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 2:
        GsSortFlipSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 3:
        GsSortSprite(GS_SPRITE_VIEW(sprite), GS_OT_VIEW(ot), pri);
        return;
    case 4: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = POLY_G4_VIEW(packet)->x0 - origin->x;
            v[0].vy = POLY_G4_VIEW(packet)->y0 - origin->y;
            v[1].vx = POLY_G4_VIEW(packet)->x1 - origin->x;
            v[1].vy = POLY_G4_VIEW(packet)->y1 - origin->y;
            v[2].vx = POLY_G4_VIEW(packet)->x2 - origin->x;
            v[2].vy = POLY_G4_VIEW(packet)->y2 - origin->y;
            v[3].vx = POLY_G4_VIEW(packet)->x3 - origin->x;
            v[3].vy = POLY_G4_VIEW(packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&POLY_G4_VIEW(packet)->x0, (long *)&POLY_G4_VIEW(packet)->x1,
                                 (long *)&POLY_G4_VIEW(packet)->x2, (long *)&POLY_G4_VIEW(packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            Graphics_SubmitDrawModePacket((u32 *)packet, GS_OT_VIEW(ot), (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, GS_OT_VIEW(ot), pri);
        }
        return;
    }
    case 5: {
        if ((u32)sprite & 0x04000000) {
            otz = (long *)0x1F8002E0;
            v = SCRATCH_VERTEX(0);
            v[0].vx = POLY_GT4_VIEW(packet)->x0 - origin->x;
            v[0].vy = POLY_GT4_VIEW(packet)->y0 - origin->y;
            v[1].vx = POLY_GT4_VIEW(packet)->x1 - origin->x;
            v[1].vy = POLY_GT4_VIEW(packet)->y1 - origin->y;
            v[2].vx = POLY_GT4_VIEW(packet)->x2 - origin->x;
            v[2].vy = POLY_GT4_VIEW(packet)->y2 - origin->y;
            v[3].vx = POLY_GT4_VIEW(packet)->x3 - origin->x;
            v[3].vy = POLY_GT4_VIEW(packet)->y3 - origin->y;
            SCRATCH_VERTEX(3)->vz = 0;
            SCRATCH_VERTEX(2)->vz = 0;
            SCRATCH_VERTEX(1)->vz = 0;
            v[0].vz = 0;
            if (RotAverageNclip4(SCRATCH_VERTEX(0), SCRATCH_VERTEX(1), SCRATCH_VERTEX(2), SCRATCH_VERTEX(3),
                                 (long *)&POLY_GT4_VIEW(packet)->x0, (long *)&POLY_GT4_VIEW(packet)->x1,
                                 (long *)&POLY_GT4_VIEW(packet)->x2, (long *)&POLY_GT4_VIEW(packet)->x3,
                                 otz, otz + 1, otz + 2) <= 0) {
                return;
            }
        }
        if ((u32)sprite & 0x40000000) {
            Graphics_SubmitDrawModePacket((u32 *)packet, GS_OT_VIEW(ot), (u16)pri, ((u32)sprite >> 28) & 3);
        } else {
            GsSortPoly(packet, GS_OT_VIEW(ot), pri);
        }
        return;
    }
    case 0:
        return;
    default: {
        SVECTOR *v2;
        SVECTOR *v3;
        SVECTOR *v1;
        u32 attribute;

        otz = (long *)0x1F8002E0;
        v = SCRATCH_VERTEX(0);
        attribute = sprite->attribute;

        /* GsSPRITE's colour-mode bits (24-25) and semi-transparency rate
         * (28-29) go into the tpage's mode and rate fields. */
        POLY_FT4_VIEW(packet)->tpage =
            sprite->tpage | (((attribute >> 17) & 0x180) | ((attribute >> 23) & 0x60));
        if (sprite->attribute & 0x40000000) {
            SetSemiTrans(packet, 1);
        }
        POLY_FT4_VIEW(packet)->clut = (sprite->cxcy.h.cy << 6) | ((sprite->cxcy.h.cx >> 4) & 0x3F);
        if (sprite->attribute & 0x800000) {
            /* Horizontally flipped: the left and right u columns swap. */
            POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 = sprite->uv.b.lo;
            POLY_FT4_VIEW(packet)->u0 = POLY_FT4_VIEW(packet)->u2 =
                sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
            POLY_FT4_VIEW(packet)->v0 = POLY_FT4_VIEW(packet)->v1 = sprite->uv.b.hi;
            POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                sprite->uv.b.hi + sprite->extent.wh.h - 1;
        } else {
            POLY_FT4_VIEW(packet)->u0 = POLY_FT4_VIEW(packet)->u2 = sprite->uv.b.lo;
            POLY_FT4_VIEW(packet)->v0 = POLY_FT4_VIEW(packet)->v1 = sprite->uv.b.hi;
            if (sprite->attribute & 0x80) {
                POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word;
                POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h;
            } else {
                POLY_FT4_VIEW(packet)->u1 = POLY_FT4_VIEW(packet)->u3 =
                    sprite->uv.b.lo + sprite->extent.wh.w.word - 1;
                POLY_FT4_VIEW(packet)->v2 = POLY_FT4_VIEW(packet)->v3 =
                    sprite->uv.b.hi + sprite->extent.wh.h - 1;
            }
        }
        v1 = v + 1;
        v2 = v + 2;
        v3 = v + 3;
        v->vx = v[2].vx = sprite->xy.h.x - origin->x;
        v[1].vx = v[3].vx = v->vx + sprite->extent.wh.w.word;
        v->vy = v1->vy = sprite->xy.h.y - origin->y;
        v[2].vy = v3->vy = v->vy + sprite->extent.wh.h;
        v3->vz = 0;
        v2->vz = 0;
        v1->vz = 0;
        v->vz = 0;
        if (RotAverageNclip4(v, v1, v2, v3,
                             (long *)&POLY_FT4_VIEW(packet)->x0, (long *)&POLY_FT4_VIEW(packet)->x1,
                             (long *)&POLY_FT4_VIEW(packet)->x2, (long *)&POLY_FT4_VIEW(packet)->x3,
                             otz, otz + 1, otz + 2) > 0) {
            if (origin->divisions == 0) {
                GsSortPoly(packet, GS_OT_VIEW(ot), pri);
                return;
            }
            {
                u32 *rgbc = (u32 *)0x1F800280;
                u32 *uv = (u32 *)0x1F800290;
                DIVPOLYGON4 *divp = (DIVPOLYGON4 *)0x1F800000;

                divp->ndiv = origin->divisions;
                divp->pih = 320;
                divp->piv = DISPLAY_OBJECT_DIVP_PIV;
                *rgbc = *(u32 *)&POLY_FT4_VIEW(packet)->r0;
                uv[0] = *(u32 *)&POLY_FT4_VIEW(packet)->u0;
                uv[1] = *(u32 *)&POLY_FT4_VIEW(packet)->u1;
                uv[2] = *(u32 *)&POLY_FT4_VIEW(packet)->u2;
                uv[3] = *(u32 *)&POLY_FT4_VIEW(packet)->u3;
                gGraphics_pPacketWorkCursor = (u32 *)DivideFT4(v, v1, v2, v3,
                                             uv, (u32 *)0x1F800294, (u32 *)0x1F800298, (u32 *)0x1F80029C,
                                             (CVECTOR *)rgbc, (POLY_FT4 *)gGraphics_pPacketWorkCursor,
                                             (u32 *)GS_OT_VIEW(ot)->org + pri, divp);
            }
        }
        return;
    }
    }
}

void DisplayObject_InitializeGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u16 flags = object->flags;

    object->field_54 = 0;
    object->field_4C = 0;
    object->field_44.word = 0;
    object->field_3C.word = 0;
    object->field_34.word = 0;
    object->field_2C.word = 0;
    object->back_face_handler = 0;
    object->field_20.b.rotation_y = 0;
    object->field_20.b.rotation_x = 0;
    object->field_20.b.rotation_z = 0;
    object->field_1C = 0;
    object->field_18.center.y = 0;
    object->field_18.center.x = 0;
    /* List 4 reads only this low byte to gate a second POLY_G4 submission. */
    *(u8 *)&object->field_5A = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void DisplayObject_InitializeTexturedGouraudQuad(
    DisplayObject *object,
    s32 has_secondary_quad
)
{
    u32 initial = 0x00808080;
    u16 flags = object->flags;

    *(u32 *)&object->field_68 = initial;
    *(u32 *)&object->field_5C = initial;
    object->field_50.word = initial;
    object->field_44.word = initial;
    object->field_38.word = initial;
    object->field_2C.word = initial;
    object->back_face_handler = 0;
    object->field_20.b.rotation_y = 0;
    object->field_20.b.rotation_x = 0;
    object->field_20.b.rotation_z = 0;
    object->field_1C = 0;
    object->field_18.center.y = 0;
    object->field_18.center.x = 0;
    ((u8 *)object)[0x72] = has_secondary_quad;
    object->flags = flags | DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
}

void DisplayObject_ConfigureSpriteWithResource(
    DisplayObject *object,
    s32 resource_root_index,
    s32 resource_branch_index,
    s32 resource_variant,
    s32 texture_config,
    s32 palette_config,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteResource(object, resource_root_index, resource_branch_index, resource_variant, texture_config, palette_config);
}

void DisplayObject_ConfigureSpriteAtPositionWithResource(
    DisplayObject *object,
    s32 x,
    s32 y,
    s32 resource_root_index,
    s32 resource_branch_index,
    s32 resource_variant,
    s32 texture_config,
    s32 palette_config,
    void *resource
)
{
    object->field_54 = resource;
    DisplayObject_ConfigureSpriteAtPosition(object, x, y, resource_root_index, resource_branch_index, resource_variant, texture_config, palette_config);
}

s32 DisplayObject_SetDepthOffset(DisplayObject *obj, s8 value)
{
    u32 index = obj->ot_index;
    volatile u16 *table = gDisplayObject_awOrderingTableDepthBases;
    s32 result;

    obj->depth_offset = value;
    result = table[index] - value;
    obj->ot_depth = result;
    return result;
}

void DisplayObject_SelectOrderingTable1(DisplayObject *object)
{
    object->ot_index = 1;
    object->ot_depth = gDisplayObject_awOrderingTableDepthBases[1] - object->depth_offset;
}

void DisplayObject_SelectOrderingTable3(DisplayObject *object)
{
    object->ot_index = 3;
    object->ot_depth = gDisplayObject_awOrderingTableDepthBases[3] - object->depth_offset;
}

int DisplayObject_RunUpdateAndCheckRenderable(DisplayObject *object)
{
    /* Called as void (*)(void), with no argument, on purpose: the slot's
       declared type takes a u8 *, but the ambient argument register is what
       retail passes. See #2887. */
    void (*callback)(void) = (void (*)(void))object->update;

    if (callback != 0)
        callback();
    return ((object->flags & DISPLAY_OBJECT_RENDERABLE_MASK) ==
            DISPLAY_OBJECT_RENDERABLE_MASK);
}

u32 DisplayObjectStream_ReadU16LE(const u8 *data)
{
    return (data[1] << 8) | data[0];
}

u8 *DisplayObjectStream_ResolveOffset(
    DisplayObjectStream *object,
    const u8 *data
)
{
    return object->base + ((data[1] << 8) | data[0]);
}

void DisplayObject_ResetVelocity(DisplayObjectVelocity *object)
{
    if (object != 0) {
        object->velocity_x = 0;
        object->velocity_y = 0;
        object->velocity_z = 0;
        object->fraction_x = 0x80;
        object->fraction_y = 0x80;
        object->fraction_z = 0x80;
    }
}

void DisplayObject_StepPositionX(DisplayObjectVelocity *object)
{
    int value = (object->x << 8) | object->fraction_x;

    value += object->velocity_x;
    object->fraction_x = value;
    object->x = value >> 8;
}

void DisplayObject_StepPositionY(DisplayObjectVelocity *object)
{
    int value = (object->y << 8) | object->fraction_y;

    value += object->velocity_y;
    object->fraction_y = value;
    object->y = value >> 8;
}

void DisplayObject_StepPositionZ(DisplayObjectVelocity *object)
{
    int value = (object->z << 8) | object->fraction_z;

    value += object->velocity_z;
    object->fraction_z = value;
    object->z = value >> 8;
}

void DisplayObject_StepPositionXY(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
}

void DisplayObject_StepPositionXYZ(DisplayObjectVelocity *object)
{
    DisplayObject_StepPositionX(object);
    DisplayObject_StepPositionY(object);
    DisplayObject_StepPositionZ(object);
}

s32 DisplayObject_StepToward(s32 value, s32 target, s32 step)
{
    if (target < 0) {
        value -= step;
        if (value < target) {
            value = target;
        }
    } else {
        value += step;
        if (value > target) {
            value = target;
        }
    }
    return value;
}

s32 DisplayObject_StepTowardZero(s32 value, s32 step)
{
    if (value < 0) {
        value += step;
        if (value > 0) {
            value = 0;
        }
    } else {
        value -= step;
        if (value < 0) {
            value = 0;
        }
    }
    return value;
}

#define DISPLAY_OBJECT_FROM_FIELD_6C(field) \
    ((DisplayObject *)((field) - 0x6C))

void *DisplayObject_FindAllocatedByTag(s32 value)
{
    u8 *object = (u8 *)gDisplayObject_aPool;
    s32 count = DISPLAY_OBJECT_POOL_CAPACITY;
    u8 *field = (u8 *)&((DisplayObject *)object)->field_6C;

    do {
        if ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->flags &
             DISPLAY_OBJECT_FLAG_ALLOCATED) &&
            ((DISPLAY_OBJECT_FROM_FIELD_6C(field)->field_6C & 0xF) == value))
            return object;
        field += DISPLAY_OBJECT_RECORD_SIZE;
        count--;
        object += DISPLAY_OBJECT_RECORD_SIZE;
    } while (count != 0);
    return 0;
}

s32 DisplayObject_MarkInitialized(DisplayObjectLifecycle *object)
{
    if ((object->flags & DISPLAY_OBJECT_FLAG_ALLOCATED) == 0) {
        object->flags |= DISPLAY_OBJECT_FLAG_ALLOCATED;
        return 0;
    }
    return 1;
}

void DisplayObject_FadeBrightnessAndRelease(DisplayObjectLifecycle *object)
{
    s32 value = object->red - object->fade_step;

    if (value > 0) {
        object->blue = value;
        object->green = value;
        object->red = value;
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}

/* Builds the mask one quadrant step at a time. The eight POLY_G4 quads sit in
 * the scratchpad at 0x1F800020: quads 0-3 are the inner ring, black at the
 * +0x48 radius and fading to white at the +0x4A radius, and quads 4-7 are
 * the outer ring, white from the +0x4A radius out to 240. Each ring has one
 * quad per quadrant (1 and 3 on the right, 0 and 2 on the left, 1 and 0 at
 * the top). The radii are scaled by the object's +0x44/+0x46 pair, 4096 being
 * 1.0. Every pass computes the next edge from rcos/rsin of 0x100 more (a
 * sixteenth of a turn) and links all eight quads through Graphics_SubmitDrawModePacket in
 * mode 2, before that edge becomes the next pass's first one.
 *
 * The chained assignments are what reproduce retail. GCC gives each chain
 * its own block-local value register, and it lifts the addresses of quads 1,
 * 4, 5, 6 and 7 into pseudos of its own. Those are the spilled pointers at
 * sp+0x1C..0x28 and in $fp. Named pointer locals allocate differently. */
void DisplayObject_RenderSpotlightMask(DisplayObject *object, GsOT *ot)
{
    s32 depth;
    s32 inner;
    s32 outer;
    s32 *radius = (s32 *)0x1F800000;
    s32 *scale = (s32 *)0x1F800010;
    POLY_G4 *poly = (POLY_G4 *)0x1F800020;
    s32 i;
    s32 j;
    s32 cx;
    s32 cy;

    scale[0] = object->field_44.scale.x;
    scale[1] = object->field_44.scale.y;
    depth = (s16)object->ot_depth;
    cx = (s16)object->xy.h.x;
    cy = (s16)object->xy.h.y;
    inner = object->field_48.spotlight.inner;
    outer = object->field_48.spotlight.outer;

    for (i = 0; i < 4; i++) {
        *(u32 *)&poly[i].r0 = 0;
        *(u32 *)&poly[i].r2 = 0;
    }
    for (i = 4; i < 8; i++) {
        *(u32 *)&poly[i].r0 = 0xFFFFFF;
        *(u32 *)&poly[i].r2 = 0xFFFFFF;
    }
    for (i = 0; i < 8; i++) {
        poly[i].y1 = cy;
        poly[i].y0 = cy;
        *(u32 *)&poly[i].r1 = 0xFFFFFF;
        *(u32 *)&poly[i].r3 = 0xFFFFFF;
        setPolyG4(&poly[i]);
    }

    radius[0] = inner * scale[0] / 4096;
    radius[1] = outer * scale[0] / 4096;
    radius[2] = scale[0] * 240 / 4096;

    poly[3].x0 = poly[1].x0 = radius[0] + cx;
    poly[3].x1 = poly[1].x1 = poly[7].x0 = poly[5].x0 = radius[1] + cx;
    poly[7].x1 = poly[5].x1 = radius[2] + cx;
    poly[2].x0 = poly[0].x0 = cx - radius[0];
    poly[2].x1 = poly[0].x1 = poly[6].x0 = poly[4].x0 = cx - radius[1];
    poly[6].x1 = poly[4].x1 = cx - radius[2];

    for (i = 0x100; i < 0x401; i += 0x100) {
        radius[0] = inner * rcos(i) / 4096;
        radius[0] = radius[0] * scale[0] / 4096;
        radius[1] = outer * rcos(i) / 4096;
        radius[1] = radius[1] * scale[0] / 4096;
        radius[2] = rcos(i) * 240 / 4096;
        radius[2] = radius[2] * scale[0] / 4096;

        poly[3].x2 = poly[1].x2 = radius[0] + cx;
        poly[3].x3 = poly[1].x3 = poly[7].x2 = poly[5].x2 = radius[1] + cx;
        poly[7].x3 = poly[5].x3 = radius[2] + cx;
        poly[2].x2 = poly[0].x2 = cx - radius[0];
        poly[2].x3 = poly[0].x3 = poly[6].x2 = poly[4].x2 = cx - radius[1];
        poly[6].x3 = poly[4].x3 = cx - radius[2];

        radius[0] = inner * rsin(i) / 4096;
        radius[0] = radius[0] * scale[1] / 4096;
        radius[1] = outer * rsin(i) / 4096;
        radius[1] = radius[1] * scale[1] / 4096;
        radius[2] = rsin(i) * 240 / 4096;
        radius[2] = radius[2] * scale[1] / 4096;

        poly[2].y2 = poly[3].y2 = radius[0] + cy;
        poly[2].y3 = poly[3].y3 = poly[6].y2 = poly[7].y2 = radius[1] + cy;
        poly[6].y3 = poly[7].y3 = radius[2] + cy;
        poly[0].y2 = poly[1].y2 = cy - radius[0];
        poly[0].y3 = poly[1].y3 = poly[4].y2 = poly[5].y2 = cy - radius[1];
        poly[4].y3 = poly[5].y3 = cy - radius[2];

        for (j = 0; j < 8; j++) {
            Graphics_SubmitDrawModePacket((u32 *)&poly[j], ot, (u16)depth, 2);
            *(s32 *)&poly[j].x0 = *(s32 *)&poly[j].x2;
            *(s32 *)&poly[j].x1 = *(s32 *)&poly[j].x3;
        }
    }
}

void DisplayObject_SavePosition(DisplayObjectSnapshot *object)
{
    object->field_36 = object->field_30;
    object->field_38 = object->field_32;
}

void DisplayObject_InterpolatePositionCosine(
    DisplayObjectPosition *object, s32 arg1, s32 arg2, s32 arg3
)
{
    s32 x = (object->x + arg1) >> 1;
    s32 y = (object->y + arg2) >> 1;
    s32 scale = rcos(arg3);
    s32 scale2 = scale;

    object->out_x = x + (object->x - x) * scale / ONE;
    object->out_y = y + (object->y - y) * scale2 / ONE;
}

void Widget_SlideSine(
    DisplayObjectPosition *object,
    int target_x,
    int target_y,
    int phase
)
{
    int dx = object->x - target_x;
    int dy = object->y - target_y;
    int factor;

    if (phase < 0) {
        factor = rsin(phase + TRIG_ANGLE_QUARTER_TURN);
        object->out_x = object->x - (dx * factor) / ONE;
        object->out_y = object->y - (dy * factor) / ONE;
    } else {
        factor = -rsin(phase);
        object->out_x = target_x - (dx * factor) / ONE;
        object->out_y = target_y - (dy * factor) / ONE;
    }
}
