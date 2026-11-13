#include "../types.h"

/* The duel card preview, one translation unit from CardPreview_HandleResourceTransfer
 * (US 0x800289BC) through DuelEffect_ClearResourceObjectPointers: the preview
 * resource transfer stage, the detail panel draw callback, the facing variant
 * update, and the request, creation and release of the preview resource
 * objects. */

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "duel_scene.h"
#include "card_preview.h"

#define VRAM_UPLOAD_WORDS(address) ((u32 *)(address))

/* Regional values: the package size and the offset of its last image. The
   European package holds one 0x2A0-byte third image per text language
   (gText_bLanguageIndex, below), so the fourth image moves from 0x2AE0 to 0x3560.
   The European build (src/game/european/) defines its own. */
#ifndef CARD_PREVIEW_TRANSFER_PHASE_SIZE
#define CARD_PREVIEW_TRANSFER_PHASE_SIZE 0x3800
#endif
#ifndef CARD_PREVIEW_FOURTH_IMAGE_OFFSET
#define CARD_PREVIEW_FOURTH_IMAGE_OFFSET 0x2AE0
#endif

void CardPreview_HandleResourceTransfer(FileTransferDescriptor *object, s32 mode)
{
    DuelEffectResourceRecord *e;
    RECT *rect;
    u8 *image_source;
    s32 x;
#ifdef VERSION_EUROPE
    s32 offset;
#endif

    if (mode == 0) {
        object->phase_size = CARD_PREVIEW_TRANSFER_PHASE_SIZE;
        gFile_dwTransferState &= 0xFFDCFFFF;
        object->alternate_sector_destination = (u32)gFile_TransferBufferBase;
        object->sector_destination = (u32)gFile_TransferBufferBase;
        object->transfer_mode = 1;
        return;
    }

    e = &gCardPreview_Resources[(s32)object->callback_data];

    rect = &e->rects[0];
    image_source = gFile_TransferBufferBase;
    e->rects[0].w = 0x33;
    e->rects[0].h = 0x60;
    e->rects[0].x = e->src_x;
    e->rects[0].y = e->src_y;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[1];
    x = e->clut_x;
    image_source = gFile_TransferBufferBase + 0x2640;
    e->rects[1].w = 0x100;
    e->rects[1].h = 1;
    e->rects[1].x = x;
    e->rects[1].y = e->clut_y;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[2];
#ifdef VERSION_EUROPE
    /* The third image of the current text language. */
    offset = gText_bLanguageIndex * 0x2A0 + 0x2840;
    image_source = gFile_TransferBufferBase + offset;
    e->rects[2].w = 0x18;
    e->rects[2].h = 0xE;
    e->rects[2].x = e->src_x;
#else
    x = e->src_x;
    image_source = gFile_TransferBufferBase + 0x2840;
    e->rects[2].w = 0x18;
    e->rects[2].h = 0xE;
    e->rects[2].x = x;
#endif
    e->rects[2].y = e->src_y + 0x60;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));

    rect = &e->rects[3];
    image_source = gFile_TransferBufferBase + CARD_PREVIEW_FOURTH_IMAGE_OFFSET;
    e->rects[3].w = 8;
    e->rects[3].h = 0x58;
    e->rects[3].x = e->src_x + 0x38;
    e->rects[3].y = e->src_y;
    LoadImage(rect, VRAM_UPLOAD_WORDS(image_source));
}

#include "text_encode_decimal_digits.h"
#include "display_object_system.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "sprite_primitive.h"
#include "card_constants.h"
#include "../ygo_types.h"

/* The dispatcher takes byte pointers. Converting through each record's first
 * member keeps the packet and the origin from being swapped. */
#define CARD_PANEL_PACKET_BYTES(packet) ((u8 *)&(packet)->tag)
#define CARD_PANEL_ORIGIN_BYTES(origin) ((u8 *)&(origin)->x)
/*
 * Duel card-detail panel: builds the scratchpad sprite parameters for the
 * panel frame, the card image, the ATK/DEF digit rows, the repeated level
 * icons and the attribute tile, and submits each through
 * DisplayObject_SubmitPacket. Byte-exact under gcc_2_8_1_g8_split with no
 * register pins, aliases or inline assembly.
 *
 * Three constructs exist only to steer the compiler and emit no code:
 *
 *  - `sa`/`sb` with the 0x00090009 extent inside a do/while (0): the loop
 *    note after the x load is a scheduling barrier, which gives retail's
 *    `lhu; lui; ori; addiu` order at the level-icon block.
 *
 *  - the two guarded write-backs (`win->flags = flags;` and the second
 *    `PRM->cxcy.h.cy = white;`). Each stores a value its destination already
 *    holds. cse keeps such a store, so the guard and its condition are real
 *    code for flow, both scheduling passes and both register allocators;
 *    reload's cse then deletes the no-op store, and jump2 deletes the empty
 *    branch together with the whole condition chain. Nothing reaches the
 *    object, but while they exist they shape the conflict graph:
 *      - the first makes the 14 (`k`) and the 0xFEFFFFFF mask (`m`) global
 *        pseudos that conflict with each other and with the two temporaries
 *        in v0/v1, so global-alloc gives `k` t1 and `m` t2 as retail has
 *        (local-alloc alone puts the 14 in v0 and the mask in t1). `k` and
 *        `m` are read before their only assignment on purpose: a second
 *        assignment would stop `li k` being placed next to its store and
 *        lift it above the argument moves. `k` is u16 because an SImode 14
 *        lets reload's cse rewrite `addiu t0,t0,14` as `addu t0,t0,t1`.
 *      - the second keeps the 0xF8 (`white`) live while the clamp counter
 *        `i` holds s0, so `white` takes s4. Its condition repeats the shape
 *        of the clamp test that follows, which keeps local-alloc's order
 *        for the two field loads.
 *    With those registers the second scheduling pass floats `li t1,14` and
 *    `li s4,248` to the top of the second submit block by itself.
 */
/* Regional values: the panel's CLUT row and column, the second frame
   sprite's semi-transparency bits, and where the ATK and DEF digit rows sit.
   The European build (src/game/european/) defines its own. */
#ifndef CARD_PREVIEW_PANEL_CLUT_Y
#define CARD_PREVIEW_PANEL_CLUT_Y 0xF8
#endif
#ifndef CARD_PREVIEW_PANEL_CLUT_X
#define CARD_PREVIEW_PANEL_CLUT_X 0x1C0
#endif
#ifndef CARD_PREVIEW_PANEL_FRAME_ATTRIBUTE
#define CARD_PREVIEW_PANEL_FRAME_ATTRIBUTE 0x60000000
#endif
#ifndef CARD_PREVIEW_PANEL_ATK_X
#define CARD_PREVIEW_PANEL_ATK_X 0x61
#endif
#ifndef CARD_PREVIEW_PANEL_ATK_Y
#define CARD_PREVIEW_PANEL_ATK_Y 0x9D
#endif
#ifndef CARD_PREVIEW_PANEL_DEF_X
#define CARD_PREVIEW_PANEL_DEF_X 0x61
#endif
#ifndef CARD_PREVIEW_PANEL_DEF_Y
#define CARD_PREVIEW_PANEL_DEF_Y 0xAB
#endif

void CardPreview_DrawDetailPanel(DisplayObject *obj, s32 ordering_table) {
    u8 buf1[5];
    u8 buf2[5];
    DisplayObjectPacketOrigin *origin;
    SpritePrim *PRM;
    POLY_FT4 *packet;
    DisplayObject *win;
    DuelEffectResourceRecord *rec;
    s32 arg;
    s32 i;
    u16 flags;
    u32 f4;
    u32 tile;
    u32 lo;
    s32 wrap;
    u16 k;
    s32 white;
    s32 sa;
    s32 sb;
    u32 m;

    wrap = 0xFFFF;
    win = (DisplayObject *)obj->field_54;
    if ((obj->attribute & (1<<31) ) != 0) {
        return;
    }
    flags = win->flags;
    if ((obj->texture_page & m) ^ (obj->resource_root_index & k)) {
        win->flags = flags;
    }
    if ((flags & 0x40) == 0) {
        return;
    }
    origin = (DisplayObjectPacketOrigin *)0x1F800398;
    PRM = (SpritePrim *)0x1F800320;
    packet = (POLY_FT4 *)0x1F800344;
    arg = (((s16)win->ot_depth - 1) & 0xFFFF) | 0x10000;
    if (flags & 0x4) {
        obj->field_20.word = win->field_20.word;
        f4 = obj->attribute & ~(1<<27) ;
        obj->field_44.word = win->field_44.word;
        obj->attribute = f4;
        f4 = f4 | (win->attribute & 0x08000000);
        obj->attribute = f4;
        if (DisplayObject_ProjectRotationFacing(
                (struct DisplayObject *)obj, (s16)win->xy.h.x + (s16)win->field_18.center.x,
                (s16)win->xy.h.y + (s16)win->field_18.center.y, (struct ProjectionOut *)origin
            ) <= 0) {
            return;
        }
        arg = (((s16)win->ot_depth - 1) & 0xFFFF) | 0xF0000;
        *(u32 *)&packet->r0 = win->rgb;
        setlen(packet, 9);
        packet->code = 0x2C;
    }

    PRM->attribute = obj->attribute;
    PRM->xy.h.x = win->xy.h.x + 0x13;
    PRM->xy.h.y = win->xy.h.y + 0x32;
    PRM->extent.wh.w.word = 0x66;
    PRM->extent.wh.h = 0x60;
    PRM->rgb = win->rgb;
    PRM->cxcy.word = obj->field_40.word;
    PRM->uv.word = obj->texture_uv;
    PRM->tpage = obj->texture_page;
    DisplayObject_SubmitPacket(
        PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
        CARD_PANEL_ORIGIN_BYTES(origin));

    packet->code |= 2;
    PRM->xy.h.x = win->xy.h.x + 0xC;
    PRM->xy.h.y = win->xy.h.y + 0xE;
    PRM->uv.b.hi = PRM->uv.b.hi + 0x60;
    m = 0xFEFFFFFF;
    PRM->attribute = (PRM->attribute & m) | CARD_PREVIEW_PANEL_FRAME_ATTRIBUTE;
    PRM->extent.wh.w.word = 0x60;
    k = 0xE;
    PRM->extent.wh.h = k;
    PRM->cxcy.h.cx = CARD_PREVIEW_PANEL_CLUT_X + 0x20;
    white = CARD_PREVIEW_PANEL_CLUT_Y;
    PRM->cxcy.h.cy = white;
#ifdef VERSION_EUROPE
    /* The European frame is drawn at three quarters of the object's colour,
       which is then restored. */
    ((u8 *)&PRM->rgb)[0] = packet->r0 = ((u8 *)&obj->rgb)[0] * 3u >> 2;
    ((u8 *)&PRM->rgb)[1] = packet->g0 = ((u8 *)&obj->rgb)[1] * 3u >> 2;
    ((u8 *)&PRM->rgb)[2] = packet->b0 = ((u8 *)&obj->rgb)[2] * 3u >> 2;
    DisplayObject_SubmitPacket(
        PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
        CARD_PANEL_ORIGIN_BYTES(origin));
    packet->r0 = ((u8 *)&obj->rgb)[0];
    packet->g0 = ((u8 *)&obj->rgb)[1];
    packet->b0 = ((u8 *)&obj->rgb)[2];
    PRM->rgb = win->rgb;
    PRM->attribute = (PRM->attribute & 0x8FFFFFFF) | 0x60000000;
#else
    DisplayObject_SubmitPacket(
        PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
        CARD_PANEL_ORIGIN_BYTES(origin));
#endif

    origin->divisions = 0;
    rec = &gCardPreview_Resources[obj->resource_root_index];
    PRM->tpage = 0x1F;
    PRM->cxcy.h.cx = PRM->cxcy.h.cx + 0x10;
    PRM->xy.h.x = win->xy.h.x + obj->xy.h.x;
    PRM->xy.h.y = win->xy.h.y + obj->xy.h.y;
    PRM->extent.word = obj->field_3C.word;
    PRM->uv.word = obj->resource_uv;
    if (obj->resource_branch_index < 0x14) {
        if (rec->stat_palette_flags & 0x80) {
            PRM->cxcy.h.cy = PRM->cxcy.h.cy + 1;
        }
        DisplayObject_SubmitPacket(
            PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
            CARD_PANEL_ORIGIN_BYTES(origin));
        PRM->cxcy.h.cy = white;
#ifdef VERSION_EUROPE
        PRM->uv.b.lo = PRM->uv.b.lo + 32;
        PRM->xy.h.x = PRM->xy.h.x + 51;
#else
        PRM->uv.b.hi = PRM->uv.b.hi + *(u8 *)&PRM->extent.wh.h;
        PRM->xy.h.y = PRM->xy.h.y + (PRM->extent.wh.h + wrap);
#endif
        if (rec->stat_palette_flags & 0x40) {
            PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y + 1;
        }
        DisplayObject_SubmitPacket(
            PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
            CARD_PANEL_ORIGIN_BYTES(origin));
        PRM->cxcy.h.cy = white;

        i = rec->attack + rec->attack_adjustment;
        if (i > 9999) {
            PRM->cxcy.h.cy = white;
        }
        if (i > 9999 ) {
            i = 9999 ;
        }
        Text_EncodeDecimalDigits(i, 4, buf1);
        i = rec->defense + rec->defense_adjustment;
        if (i > 9999 ) {
            i = 9999 ;
        }
        Text_EncodeDecimalDigits(i, 4, buf2);

        PRM->uv.b.hi = (PRM->uv.b.hi & 0x80) + 0x10;
        PRM->xy.h.x = win->xy.h.x + CARD_PREVIEW_PANEL_ATK_X;
        PRM->xy.h.y = win->xy.h.y + CARD_PREVIEW_PANEL_ATK_Y;
        *(u32 *)&PRM->extent = 0x000D0006;
        if (rec->stat_palette_flags & 0x80) {
            PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y + 1;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf1[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(
                PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
                CARD_PANEL_ORIGIN_BYTES(origin));
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        PRM->xy.h.x = win->xy.h.x + CARD_PREVIEW_PANEL_DEF_X;
        PRM->xy.h.y = win->xy.h.y + CARD_PREVIEW_PANEL_DEF_Y;
        PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y;
        if (rec->stat_palette_flags & 0x40) {
            PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y + 1;
        }
        i = 3;
        do {
            PRM->uv.b.lo = buf2[i] * 6 + 0x10;
            DisplayObject_SubmitPacket(
                PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
                CARD_PANEL_ORIGIN_BYTES(origin));
            PRM->xy.h.x = PRM->xy.h.x + 6;
            i--;
        } while (i >= 0);

        sa = win->xy.h.x;
        do { sb = 0x00090009; } while (0);
        PRM->xy.h.x = sa + 0x77;
        PRM->xy.h.y = win->xy.h.y + 0x20;
        *(u32 *)&PRM->extent = sb;
        PRM->uv.b.lo = 0;
        PRM->cxcy.h.cx = CARD_PREVIEW_PANEL_CLUT_X;
        PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y;
        if (rec->level != 0) {
            i = 0;
            do {
                DisplayObject_SubmitPacket(
                    PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
                    CARD_PANEL_ORIGIN_BYTES(origin));
                PRM->xy.h.x = PRM->xy.h.x - 9;
                i++;
            } while (i < (s32)rec->level);
        }
    } else {
        DisplayObject_SubmitPacket(
            PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
            CARD_PANEL_ORIGIN_BYTES(origin));
    }

    PRM->xy.h.x = win->xy.h.x + 0x6E;
    *(u32 *)&PRM->extent = 0x00100010;
    PRM->xy.h.y = win->xy.h.y + 0xD;
    lo = rec->attribute_icon << 4;
    PRM->uv.b.lo = lo;
    PRM->uv.b.hi = PRM->uv.b.hi & 0x80;
    tile = PRM->uv.b.lo;
    PRM->cxcy.h.cx = win->field_40.clut.x + tile;
    PRM->cxcy.h.cy = CARD_PREVIEW_PANEL_CLUT_Y + 7;
    DisplayObject_SubmitPacket(
        PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,
        CARD_PANEL_ORIGIN_BYTES(origin));
}

#include "display_object.h"

void CardPreview_UpdateVariant(DisplayObject *source, s32 facing_area)
{
    int requested_variant = 2;

    if (facing_area >= 0) {
        requested_variant = source->field_6A;
    }
    if (requested_variant != source->resource_variant) {
        DisplayObject_SetResourceVariant(source, requested_variant);
        DisplayObject_UpdateCommandStream(source);
        gDisplayObject_fRenderRetry = 1;
    }
}

#define DISPLAY_OBJECT_RESOURCE_UV_BYTES(object) ((u8 *)&(object)->resource_uv)
#include "card_constants.h"
#include "file_transfer.h"
#include "display_object.h"
#include "duel_card.h"

#ifndef DUEL_RESOURCE_TRANSFER_POSITION
#define DUEL_RESOURCE_TRANSFER_POSITION(value) \
    (((value) - 1) * 7 + CARD_COUNT)
#endif
#ifndef DUEL_RESOURCE_TRANSFER_SECTORS
#define DUEL_RESOURCE_TRANSFER_SECTORS 7
#endif
#ifndef DUEL_RESOURCE_DEFAULT_X
#define DUEL_RESOURCE_DEFAULT_X 0x48
#endif
#ifndef DUEL_RESOURCE_VARIANT_BYTE
#define DUEL_RESOURCE_VARIANT_BYTE 1
#endif
#ifndef DUEL_RESOURCE_VARIANT_17_DELTA
#define DUEL_RESOURCE_VARIANT_17_DELTA 0x20
#endif
#ifndef DUEL_RESOURCE_VARIANT_15_DELTA
#define DUEL_RESOURCE_VARIANT_15_DELTA 0x10
#endif
#ifndef DUEL_RESOURCE_PATH_Y
#define DUEL_RESOURCE_PATH_Y 0x9E
#endif
#ifndef DUEL_RESOURCE_PATH_FIELD_5E
#define DUEL_RESOURCE_PATH_FIELD_5E 0xCE
#endif
#ifndef DUEL_RESOURCE_PATH_WIDTH
#define DUEL_RESOURCE_PATH_WIDTH 0x18
#endif
#ifndef DUEL_RESOURCE_PATH_HEIGHT
#define DUEL_RESOURCE_PATH_HEIGHT 0xC
#endif
#ifndef DUEL_RESOURCE_SECONDARY_X
#define DUEL_RESOURCE_SECONDARY_X 4
#endif
#ifndef DUEL_RESOURCE_SECONDARY_Y
#define DUEL_RESOURCE_SECONDARY_Y 0x62
#endif

FileTransferDescriptor *CardPreview_RequestResourceTransfer(s32 slot, s32 card_id)
{
    FileTransferDescriptor *object;

    gCardPreview_Resources[slot].card_id = card_id;
    object = File_TryRequestAsyncTransfer(
        0, 0, DUEL_RESOURCE_TRANSFER_POSITION(card_id),
        DUEL_RESOURCE_TRANSFER_SECTORS, CardPreview_HandleResourceTransfer, 0, 0);
    object->callback_data = (void *)slot;
    gFile_dwTransferState =
        object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE;
    return object;
}

/*
 * Retail leaves the primary preview object in $v0 and callers consume that
 * pointer. The stat overrides feed the default path before setup is reused;
 * a negative signed low halfword selects the packed base-stat fallback.
 * The primary object is published at record offset zero
 * and linked from the secondary object at offset four.
 */
DisplayObject *CardPreview_CreateResourceObjects(s32 index, s32 attack, s32 defense)
{
    DuelEffectResourceRecord *entry;
    DisplayObject *object;
    s32 variant;
    s32 setup;
    s32 card_id;
    u8 byte_value;

    setup = attack;
    entry = &gCardPreview_Resources[index];

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    DisplayObject_ConfigureScreenSprite(object, 0, 0, 0x66, 0x60, 0, 0,
        ((s16)entry->src_x >> 6) + (((s16)entry->src_y >> 8) << 4),
        (s16)entry->clut_x, (s16)entry->clut_y);

    object->xy.word = 0x001F0048;
    object->field_3C.word = 0x00100038;
#ifdef VERSION_EUROPE
    object->attribute |= 0x01000000;
    object->resource_uv = ((gText_bLanguageIndex << 4) + 0x9E) << 8;
#else
    object->resource_uv = 0x9E00;
    object->attribute |= 0x01000000;
#endif

    card_id = (s16)entry->card_id;
    variant = (gDuel_adwCardStats[card_id - 1] >> 26) & 0x1F;
    object->resource_root_index = index;
    object->resource_branch_index = (u8)variant;

    switch (variant) {
    case 0x16:
        setup = 0x103;
        variant = 1;
        break;

    case 0x17:
        byte_value = DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[
            DUEL_RESOURCE_VARIANT_BYTE];
        byte_value = (u8)(byte_value + DUEL_RESOURCE_VARIANT_17_DELTA);
        DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
            byte_value;
        /* fall through */
    case 0x14:
        setup = 0x101;
        variant = 1;
        break;

    case 0x15:
        setup = 0x102;
        byte_value = DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[
            DUEL_RESOURCE_VARIANT_BYTE];
        variant = 1;
        byte_value = (u8)(byte_value + DUEL_RESOURCE_VARIANT_15_DELTA);
        DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
            byte_value;
        break;

    default:
        object->xy.h.x = DUEL_RESOURCE_DEFAULT_X;
        object->xy.h.y = DUEL_RESOURCE_PATH_Y;
        DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[DUEL_RESOURCE_VARIANT_BYTE] =
            DUEL_RESOURCE_PATH_FIELD_5E;
        object->field_3C.size.w = DUEL_RESOURCE_PATH_WIDTH;
        object->field_3C.size.h = DUEL_RESOURCE_PATH_HEIGHT;
#ifdef VERSION_EUROPE
        DISPLAY_OBJECT_RESOURCE_UV_BYTES(object)[1] =
            (gText_bLanguageIndex << 4) - 0x60;
#endif

        {
            s16 attack_value = setup;
            entry->attack = attack_value;
            if (attack_value < 0) {
                entry->attack =
                    (s16)((gDuel_adwCardStats[card_id - 1] & 0x1FF) * 10);
            }
        }
        {
            s16 defense_value = defense;
            entry->defense = defense_value;
            if (defense_value < 0) {
                entry->defense =
                    (s16)(((gDuel_adwCardStats[card_id - 1] >> 9) & 0x1FF) * 10);
            }
        }
        setup = 0x100;
        variant = 0;
        entry->defense_adjustment = 0;
        entry->attack_adjustment = 0;
        break;
    }

    entry->attribute_icon = (u8)(gDuel_abCardLevelAttr[card_id] >> 4);
    entry->level = (u8)(gDuel_abCardLevelAttr[card_id] & 0xF);
    entry->stat_palette_flags = 0;

    object->field_4C = (s32)CardPreview_DrawDetailPanel;
    DisplayObject_SelectOrderingTable1(object);

    entry->secondary_object = object;

    object = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
    DisplayObject_ConfigureSpriteAtPosition(
        object, 2, DUEL_RESOURCE_SECONDARY_X, 1, 0, variant, 0x1C,
        setup + 8);

    object->field_18.center.x = 0x46;
    object->field_48.pivot.x = 0x46;
    object->field_18.center.y = DUEL_RESOURCE_SECONDARY_Y;
    object->field_48.pivot.y = DUEL_RESOURCE_SECONDARY_Y;
    object->field_6A = (u8)variant;
    object->attribute |= 0x01000000;
    object->flags |= 8;

    DisplayObject_SelectOrderingTable1(object);
    DisplayObject_SetDepthOffset(object, -1);

    ((DisplayObject *)entry->secondary_object)->subdivisions = 3;
    object->subdivisions = 3;
    object->back_face_handler = (u32)CardPreview_UpdateVariant;

    {
        DisplayObject *secondary = entry->secondary_object;
        DisplayObject *primary;

        primary = object;

        entry->primary_object = primary;
        secondary->field_54 = primary;
    }
    return object;
}

void DuelEffect_ReleaseResourceObjects(s32 index)
{
    DuelEffectResourceRecord *entry = &gCardPreview_Resources[index];

    DisplayObject_ReleaseIfPresent(entry->primary_object);
    DisplayObject_ReleaseIfPresent(entry->secondary_object);
    entry->secondary_object = 0;
    entry->primary_object = 0;
}

void DuelEffect_ClearResourceObjectPointers(int index)
{
    DuelEffectResourceRecord *entry = &gCardPreview_Resources[index];

    entry->secondary_object = 0;
    entry->primary_object = 0;
}
