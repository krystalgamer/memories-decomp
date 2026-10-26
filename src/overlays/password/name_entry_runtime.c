/* The name-entry keyboard updater, at the end of this unit, samples each pad
 * word independently; the other name-entry functions read no pad state. */
#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/color_constants.h"
#include "../../game/input.h"
#include "../../game/display_object_config.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_core.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/text_box_update_channels.h"
#include "../../game/duel_effect_set_entry_range_layout.h"
#include "../../game/gpu_packets.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
#include "../../game/player_name_glyphs.h"
#include "../../game/fade.h"
#include "name_entry_keyboard.h"
#include "name_entry_frame.h"
#include "name_entry_state.h"
#include "dialog_choice_ref.h"
#include "../../game/rand_get_interval.h"
#include "../../game/text_box_lifecycle.h"
#include "../../game/dialog_choice_state.h"
#include "../../game/display_object_interpolation.h"
#include "../../game/text_sjis_to_glyph_codes.h"
#include "../../game/duel_effect_entry_occupancy.h"
#include "../../game/duel_effect_resource_setup.h"
#include "../../overlays/password/name_entry_keyboard.h"
#include "../../overlays/password/name_entry_frame.h"
#include "../../overlays/password/name_entry_state.h"
#include "../../overlays/password/dialog_choice_ref.h"
#include "name_entry_tables.h"

#ifdef VERSION_EUROPE
#define NAME_ENTRY_FRAGMENT_BOTTOM 0x100
#define NAME_ENTRY_SHATTER_WIDTH 0x0C
#else
#define NAME_ENTRY_FRAGMENT_BOTTOM 0xF0
#define NAME_ENTRY_SHATTER_WIDTH 0x10
#endif

/* The name-entry screen pipeline in executable order, up to the keyboard:
   setup and selection-frame drawing, then glyph lookup and effects. The
   eleven functions share one compiler profile and the gNameEntry_bStateFlags state
   block; the leading display-object helper has no proven ownership in this
   lifecycle. The keyboard handler that follows, NameEntry_UpdateKeyboard,
   is the binding-free matching C function at the end of this unit; the
   dialog and completion handling after it are in name_entry_dialog.c. */

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_BUILD_KEYBOARD_TEXT_BOX) || \
    defined(VERSION_EUROPE_PASSWORD_BUILD_KEYBOARD_TEXT_BOX)
void NameEntry_BuildKeyboardTextBox(s32 textOffset)
{
    DuelEffectChannel *object;

#ifdef VERSION_EUROPE
    object = TextBox_CreateFlagged(
        1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0x100, 3);
#else
#ifndef VERSION_JAPAN
    func_8003B6AC(1, 1);
#endif
    object = TextBox_Create(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0);
#endif
    object->glyph_advance_x = 0x14;
    object->line_advance_y = 0x12;
    func_80039A14((struct DuelEffectChannel *)object);
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_DRAW_SELECTION_FRAME) || \
    defined(VERSION_EUROPE_PASSWORD_DRAW_SELECTION_FRAME)
void NameEntry_DrawSelectionFrame(NameEntrySelectionFrameView *r, GsOT *ot)
{
    LINE_F3 *poly;
    LINE_G2 *line;
    s32 pri;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    s32 xm1;
    s32 xp3;
    s32 right;

    poly = (LINE_F3 *)0x1F800000;
    line = (LINE_G2 *)0x1F800040;
    /* Keep packed color writes; the constructors fill command bytes afterward. */
    *(u32 *)&poly->r0 = 0x0000FF00;
    *(u32 *)&line->r0 = 0x0000FF00;
    *(u32 *)&line->r1 = 0;
    /* Keep the signed load; narrow only at the packet-submit boundary. */
    pri = r->priority;
    x = r->x;
    y = r->y;
    w = r->width;
    h = r->height;
    setLineF3(poly);
    setLineG2(line);
    xm1 = x - 1;
    xp3 = x + 3;
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    poly->y2 = y - 1;
    poly->y1 = y - 1;
    poly->y0 = y + 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    right = x + w;
    poly->x1 = right + 1;
    poly->x0 = right + 1;
    poly->x2 = right - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->y2 = y + h + 1;
    poly->y1 = y + h + 1;
    poly->y0 = y + h - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    line->x1 = x + (w >> 1);
    line->x0 = x + (w >> 1);
    line->y0 = y + 2;
    line->y1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y0 = y + h - 2;
    line->y1 = 192;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y1 = y + (h >> 1);
    line->y0 = y + (h >> 1);
    line->x0 = x + 2;
    line->x1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->x0 = right - 2;
    line->x1 = 320;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_INIT) || \
    defined(VERSION_EUROPE_PASSWORD_INIT)
void NameEntry_Init(void)
{
    DisplayObject *obj;
    DuelEffectChannel *boxes;
    DuelEffectChannel *sprite;

    SD_BGMPlay(28688);
    DisplayObject_Reset();
    gNameEntry_bStateFlags = 0;
    gNameEntry_bGlyphVramSlot = 0;
    gNameEntry_bGlyphSequence = 0;
    gNameEntry_pNameBuffer = gSaveData_aPlayerNameSjis;
    Text_SjisToGlyphCodes(gText_aCurrentPlayerNameGlyphs, gSaveData_aPlayerNameSjis,
                  SAVE_DATA_PLAYER_NAME_CHAR_COUNT);
#ifdef VERSION_EUROPE
    TextBox_CreateFlagged(3, 254, 112, 204, 96, 16, 3);
    func_80039A14((struct DuelEffectChannel *)&D_800EB1C0);
#else
#ifndef VERSION_JAPAN
    func_8003B6AC(3, 1);
#endif
    TextBox_Create(3, 254, 112, 204, 96, 16);
#ifdef VERSION_JAPAN
    func_80039A14((struct DuelEffectChannel *)D_800EB0F8);
#else
    boxes = D_800EB0F8;
    boxes[3].glyph_advance_x = 16;
    boxes[3].line_advance_y = 16;
    func_80039A14((struct DuelEffectChannel *)&boxes[3]);
    func_8003B6AC(0, 1);
#endif
#endif

#ifdef VERSION_EUROPE
    obj = DisplayObject_ConfigureScreenSprite(
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1),
        202, 169, 16, 16, 104, 88, 22, 640, 226);
    obj->attribute |= 0x1000000;
    gNameEntry_apKeySprites[0] = obj;

    obj = DisplayObject_ConfigureScreenSprite(
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1),
        222, 169, 16, 16, 104, 112, 22, 640, 226);
    obj->attribute |= 0x1000000;
    gNameEntry_apKeySprites[1] = obj;

    obj = DisplayObject_ConfigureScreenSprite(
        DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1),
        265, 170, 32, 16, 88, gText_bLanguageIndex << 4, 22, 640, 226);
    obj->attribute |= 0x1000000;
    gNameEntry_apKeySprites[2] = obj;
#else
    sprite = TextBox_Create(0, 243, 262, 60, 100, 100);
    sprite->glyph_advance_x = 20;
    sprite->line_advance_y = 18;
    func_80039A14((struct DuelEffectChannel *)sprite);
#endif
#ifdef VERSION_JAPAN
    gNameEntry_bCharacterSet = 0;
    NameEntry_BuildKeyboardTextBox(0);
#else
    gNameEntry_bCharacterSet = 2;
    NameEntry_BuildKeyboardTextBox(2);
#endif
    gNameEntry_bSavedRow = 0;
    gNameEntry_bKeyboardRow = 0;
    gNameEntry_bKeyboardColumn = 0;
    gNameEntry_bCaretPos = 0;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 6);
    gNameEntry_wCursorTargetX = 22;
    obj->field_30.h.field_30 = 22;
    gNameEntry_wCursorTargetY = 24;
    obj->field_30.h.field_32 = 24;
    obj->field_3C.h.field_3C = 16;
    obj->field_3C.h.field_3E = 16;
    DisplayObject_SelectOrderingTable1(obj);
    DisplayObject_SetDepthOffset(obj, 10);
    obj->field_4C = (s32)NameEntry_DrawSelectionFrame;
    gNameEntry_pSelectionFrame = (SelectionFrame *)obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
#ifdef VERSION_EUROPE
    DisplayObject_ConfigureScreenSprite(obj, 107, 199, 32, 32, 144, 128, 23, 640, 224);
#else
    DisplayObject_ConfigureScreenSprite(obj, 107, 199, 32, 32, 144, 128, 23, 256, 240);
#endif
    obj->field_48.h.field_4A = 13;
    obj->field_48.h.field_48 = 13;
    obj->attribute = obj->attribute | GsALON;
    DisplayObject_SelectOrderingTable1(obj);
    DisplayObject_SetDepthOffset(obj, 10);
    gNameEntry_pCaret = (u8 *)obj;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 3);
#ifdef VERSION_EUROPE
    DisplayObject_ConfigureScreenSprite(obj, 0, 0, 320, 256, 0, 0, 20, 640, 227);
#else
    DisplayObject_ConfigureScreenSprite(obj, 0, 0, 320, 240, 0, 0, 20, 256, 243);
#endif
    obj->rgb = COLOR_RGB24_DIM_GREY;
    obj->attribute = obj->attribute | 0x1000000;
    DisplayObject_SelectOrderingTable3(obj);
    gNameEntry_wPendingDialogId = 244;
    Fade_WaitIn();
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_GET_GLYPH_AT) || \
    defined(VERSION_EUROPE_PASSWORD_GET_GLYPH_AT)
DuelEffectEntry *TextBox_GetGlyphAt(s32 index, s32 x, s32 y)
{
#if defined(VERSION_JAPAN)
    JapaneseDuelEffectEntry *node;
#elif defined(VERSION_EUROPE)
    DuelEffectChannel *base;
    EuropeanDuelEffectEntry *node;
#else
    DuelEffectChannel *base;
    DuelEffectEntry *node;
#endif

#if defined(VERSION_JAPAN)
    node = (JapaneseDuelEffectEntry *)
        ((DuelEffectChannel *)&((JapaneseDuelEffectChannel *)D_800EB0F8)[index])->entry_head_24;
#elif defined(VERSION_EUROPE)
    base = D_800EB0F8;
    node = (EuropeanDuelEffectEntry *)base[index].entry_head_24;
#else
    base = D_800EB0F8;
    node = base[index].entry_head_24;
#endif
    for (;;) {
#ifdef VERSION_EUROPE
        if (!(node->flags_11 & 0x80)) {
            return (DuelEffectEntry *)0;
        }
        if ((s16)node->local_x == x &&
            (s16)node->local_y == y) {
            return (DuelEffectEntry *)node;
        }
#else
        if (!(((DuelEffectEntry *)node)->flags_11 & 0x80)) {
            return (DuelEffectEntry *)0;
        }
        if (((DuelEffectEntry *)node)->x_0C == x &&
            ((DuelEffectEntry *)node)->y_0E == y) {
            return (DuelEffectEntry *)node;
        }
#endif
        node++;
    }
}
#endif

/* One frame of the flash a glyph makes when it is picked on the keyboard: the
 * source node is parked off-screen for the twelve frames the sprite scales up
 * and back down, then restored. */
#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_GLYPH_PULSE) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_GLYPH_PULSE)
void NameEntry_UpdateGlyphPulse(u8 *sprite)
{
    GlyphSprite *obj = (GlyphSprite *)sprite;
#ifdef VERSION_EUROPE
    EuropeanDuelEffectEntry *source;
#else
    DuelEffectEntry *source;
#endif
    s32 step;
    s32 value;
    s16 frame;

#ifdef VERSION_EUROPE
    source = (EuropeanDuelEffectEntry *)obj->sourceGlyph;
#else
    source = obj->sourceGlyph;
#endif
    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)sprite) == 0) {
        if (source != 0) {
#ifdef VERSION_EUROPE
            obj->savedSourceX = source->local_x;
            source->local_x = 0x400;
#else
            obj->savedSourceX = source->x_0C;
            source->x_0C = 0x400;
#endif
        }
        obj->frame = 0;
        obj->flags &= ~GsROTOFF;
    }
    value = obj->frame;
    if (value >= 12) {
#ifdef VERSION_EUROPE
        ((DisplayObject *)obj)->field_6C = 0;
        ((DisplayObject *)obj)->update = 0;
        obj->flags |= 0x08000000;
        if (((DisplayObject *)obj)->field_6B == 0) {
            DisplayObject_ReleaseIfPresent(obj);
        }
#else
        DisplayObject_ReleaseIfPresent(obj);
#endif
        return;
    }
    step = value;
    if (step >= 6) {
        step = 11 - step;
    }
    obj->scale_y = 4096 - step * 341;
    obj->scale_x = 4096 - step * 341;
    frame = obj->frame + 1;
    obj->frame = frame;
    if (frame >= 12) {
        if (source != 0) {
#ifdef VERSION_EUROPE
            source->local_x = obj->savedSourceX;
#else
            source->x_0C = obj->savedSourceX;
#endif
        }
    }
}
#endif

/* One frame of a single shard: seeded once with a random sideways kick and an
 * upward one, then thrown until it falls off the bottom of the screen. */
#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_GLYPH_FRAGMENT) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_GLYPH_FRAGMENT)
void NameEntry_UpdateGlyphFragment(DisplayObject *object)
{
    u8 flags;

    flags = object->field_6C;
    if ((flags & 0x80) == 0) {
        object->field_6C = flags | 0x80;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
        object->field_34.h.field_36 = Rand_GetInterval(0x200) - 0x100;
        object->field_38.h.field_38 = -Rand_GetInterval(0x180);
    }
    object->field_34.h.field_36 =
        DisplayObject_StepTowardZero(object->field_34.h.field_36, 8);
    object->field_38.h.field_38 =
        DisplayObject_StepToward(object->field_38.h.field_38, 0x800, 0x40);
    DisplayObject_StepPositionXY((DisplayObjectVelocity *)object);
    if ((s16)object->field_30.h.field_32 >= NAME_ENTRY_FRAGMENT_BOTTOM) {
        DisplayObject_ReleaseIfPresent(object);
    }
}
#endif

/* Breaks one 16x16 glyph into sixteen 4x4 shards, each on
 * NameEntry_UpdateGlyphFragment, and retires the glyph itself. */
#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_GLYPH_SHATTER) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_GLYPH_SHATTER)
void NameEntry_UpdateGlyphShatter(u8 *object)
{
    DisplayObject *piece;
    s32 dx;
    s32 dy;

    if (DisplayObject_MarkInitialized((DisplayObjectLifecycle *)object) == 0) {
        for (dy = 0; dy < 0x10; dy += 4) {
            for (dx = 0; dx < NAME_ENTRY_SHATTER_WIDTH; dx += 4) {
                piece = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
                if (piece != 0) {
                    /* Scoped to the copy: a function-wide view costs
                       GCC a second callee-saved register for the
                       glyph. */
                    DisplayObject *glyph = (DisplayObject *)object;

                    DisplayObject_ConfigureScreenSprite(piece,
                                  (s16)glyph->field_30.h.field_30 + dx,
                                  (s16)glyph->field_30.h.field_32 + dy,
                                  4, 4,
                                  *(u8 *)&glyph->field_5C + dx,
                                  ((u8 *)&glyph->field_5C)[1] + dy,
#ifdef VERSION_JAPAN
                                  25,
#else
                                  glyph->field_66,
#endif
                                  (u16)glyph->field_40.h.field_40,
                                  (u16)glyph->field_40.h.field_42);
                    piece->field_6C = 3;
                    piece->update =
                        (DisplayObjectCallback)NameEntry_UpdateGlyphFragment;
                }
            }
        }
    } else {
        DisplayObject_ReleaseIfPresent(object);
    }
}
#endif

/* Slides the caret to +0x44/+0x46 over the +0x60 frames it was given, then
 * snaps to the target and uninstalls itself. */
#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_CARET_TWEEN) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_CARET_TWEEN)
void NameEntry_UpdateCaretTween(DisplayObject *object)
{
    u8 flags;
    s16 remaining;

    flags = object->field_6C;
    if ((flags & 0x80) == 0) {
        object->field_6C = flags | 0x80;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)object);
        object->field_34.h.field_36 =
            ((object->field_44.h.field_44 -
              (s16)object->field_30.h.field_30) << 8) /
            object->field_60;
        object->field_38.h.field_38 =
            ((object->field_44.h.field_46 -
              (s16)object->field_30.h.field_32) << 8) /
            object->field_60;
    }
    DisplayObject_StepPositionXY((DisplayObjectVelocity *)object);
    remaining = (u16)object->field_60 - 1;
    object->field_60 = remaining;
    if (remaining <= 0) {
        object->update = 0;
        object->field_6C = 0;
        object->field_30.word = object->field_44.word;
    }
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_GLYPH_TRANSFER) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_GLYPH_TRANSFER)
/* Flies an accepted glyph from the keyboard to the name field on the same
 * tween prologue as the caret, dropping a fading copy of itself each frame.
 * On arrival it looks the destination glyph up and shatters it. */
void NameEntry_UpdateGlyphTransfer(u8 *w)
{
    DisplayObject *g = (DisplayObject *)w;
    DisplayObject *o;
    DuelEffectEntry *node;
    s32 n;

    if ((g->field_6C & 0x80) == 0) {
        g->field_6C |= 0x80;
        DisplayObject_ResetVelocity((DisplayObjectVelocity *)g);
        g->field_34.h.field_36 =
            ((g->field_44.h.field_44 - (s16)g->field_30.h.field_30) << 8) /
            g->field_60;
        g->field_38.h.field_38 =
            ((g->field_44.h.field_46 - (s16)g->field_30.h.field_32) << 8) /
            g->field_60;
    }
    if ((g->field_6C & 0x40) != 0) {
        n = (u16)g->field_60 - 1;
        g->field_60 = n;
        if ((s16)n <= 0) {
            DisplayObject_ReleaseIfPresent(g);
        }
        return;
    }
    DisplayObject_StepPositionXY((DisplayObjectVelocity *)g);
    n = (u16)g->field_60 - 1;
    g->field_60 = n;
    if ((s16)n > 0) {
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o,
                      (s16)g->field_30.h.field_30, (s16)g->field_30.h.field_32,
#ifdef VERSION_EUROPE
                      (u16)g->field_3C.h.field_3C,
                      (u16)g->field_3C.h.field_3E,
#else
                      16, 16,
#endif
                      *(u8 *)&g->field_5C, ((u8 *)&g->field_5C)[1],
                      g->field_66,
#ifdef VERSION_JAPAN
                      0x210, 0xFF);
#else
                      (u16)g->field_40.h.field_40, (u16)g->field_40.h.field_42);
#endif
        o->rgb = 0x606060;
        o->field_60 = 6;
        o->update = (DisplayObjectCallback)DisplayObject_FadeBrightnessAndRelease;
        o->attribute |= (GsALON | GsAONE);
        return;
    }
    g->field_6C |= 0x40;
    /* A non-struct store, through the union's address. As g->field_30.word
       GCC sinks it below the timer reload in the call that follows; the
       target stores the position first. */
    *(u32 *)&g->field_30 = g->field_44.word;
    /* Keep the timer reload inside the final argument; its expression
       placement controls the retail argument-setup order. It goes through
       the parameter rather than g: with every access on g, GCC gives w and
       g a callee-saved register each and the function grows by a move. */
#ifdef VERSION_EUROPE
    node = TextBox_GetGlyphAt(3, (gNameEntry_bCaretPos << 4) | 2,
#else
    node = TextBox_GetGlyphAt(3, gNameEntry_bCaretPos << 4,
#endif
                              (((DisplayObject *)w)->field_60 = 2, 0));
    if (node == 0) {
        return;
    }
    o = NameEntry_SpawnGlyphSprite(3, node);
    o->update = NameEntry_UpdateGlyphShatter;
    o->field_6C = 5;
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_SPAWN_GLYPH_SPRITE) || \
    defined(VERSION_EUROPE_PASSWORD_SPAWN_GLYPH_SPRITE)
#ifdef VERSION_JAPAN
/* The Japanese build has no Shift-JIS lookup: the node's glyph byte picks a
 * 4x16 cell of the font page, MoveImage copies it into one of sixteen VRAM
 * slots in turn, and the sprite samples that slot. */
extern RECT gNameEntry_aGlyphVramRects[];

void *NameEntry_SpawnGlyphSprite(s32 slot, DuelEffectEntry *w)
{
    GlyphSprite *obj;
    NAME_ENTRY_SPAWN_CHANNEL_TYPE *p;
    RECT *r;
    s32 c;

    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    if (w != 0) {
        r = &gNameEntry_aGlyphVramRects[gNameEntry_bGlyphVramSlot];
        c = gNameEntry_bGlyphSequence;
        r->x = ((w->glyph_index & 0xF) << 2) + 0x280;
        r->y = w->glyph_index & 0xF0;
        r->w = 4;
        r->h = 16;
        MoveImage(r, ((c & 0xF) << 2) | 0x240, (c & 0xF0) | 0x100);
        p = &((NAME_ENTRY_SPAWN_CHANNEL_TYPE *)D_800EB0F8)[slot];
        gNameEntry_bGlyphVramSlot = (gNameEntry_bGlyphVramSlot + 1) & 0xF;
        DisplayObject_ConfigureScreenSprite(
            obj, p->rect_x + w->x_0C, p->rect_y + w->y_0E,
            16, 16, (gNameEntry_bGlyphSequence & 0xF) << 4, gNameEntry_bGlyphSequence & 0xF0, 25, 0x210, 0xFF
        );
        obj->sequence = gNameEntry_bGlyphSequence;
        gNameEntry_bGlyphSequence = gNameEntry_bGlyphSequence + 1;
    } else {
        DisplayObject_ConfigureScreenSprite(
            obj,
            gNameEntry_pSelectionFrame->x, gNameEntry_pSelectionFrame->y, 16, 16, 128, 128, 23,
            256, 240
        );
    }
    obj->field_48 = 0x80008;
    obj->textBoxSlot = slot;
    obj->sourceGlyph = w;
    return obj;
}
#elif defined(VERSION_EUROPE)
void *NameEntry_SpawnGlyphSprite(s32 slot, DuelEffectEntry *w)
{
    EuropeanDuelEffectEntry *node;
    GlyphSprite *obj;
    DuelEffectChannel *p;
    s32 cell;

    node = (EuropeanDuelEffectEntry *)w;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    if (node != 0) {
        p = &D_800EB0F8[slot];
        cell = node->pad_0D[1];
        DisplayObject_ConfigureScreenSprite(
            obj,
            p->rect_x + (s16)node->local_x,
            p->rect_y + (s16)node->local_y,
            12, 16, (cell % 21) * 12, (cell / 21) * 16 + 80,
            10, 640, 192
        );
        obj->sequence = gNameEntry_bGlyphSequence;
        gNameEntry_bGlyphSequence = gNameEntry_bGlyphSequence + 1;
    } else {
        DisplayObject_ConfigureScreenSprite(
            obj,
            gNameEntry_pSelectionFrame->x, gNameEntry_pSelectionFrame->y, 16, 16, 128, 128, 23,
            640, 224
        );
    }
    obj->field_48 = 0x80008;
    obj->textBoxSlot = slot;
    obj->sourceGlyph = w;
    ((DisplayObject *)obj)->field_6B = 0;
    return obj;
}
#else
/* Makes the sprite for one glyph node: maps the node's Shift-JIS code to a
 * cell in the 16x16 font page, places it at the text box's origin plus the
 * node's local position, and stamps the spawn order into +0x6A. With no node
 * it makes the cursor sprite at gNameEntry_pSelectionFrame's position instead. */
void *NameEntry_SpawnGlyphSprite(s32 slot, DuelEffectEntry *w)
{
    /* Narrow types preserve the widening and unmasked argument setup. */
    s16 shift;
    u8 u;
    u8 v;
    s32 code;
    GlyphSprite *obj;
    DuelEffectChannel *p;
    s32 i;

    shift = 0;
    u = 0;
    v = 0;
    /* Preserve the pre-null-test read; this is not a null-safe C contract. */
    code = w->code_00;
    obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
    if (w != 0) {
        p = &D_800EB0F8[slot];
        switch (code) {
        case 0x8171: case 0x8173: code = 0x8183; break;
        case 0x8172: case 0x8174: code = 0x8184; break;
        }
        if ((u32)(code - 0x824F) < 76) {
            if ((u32)(code - 0x8259) < 7 || (u32)(code - 0x827A) < 7) {
                u = 0;
                v = 120;
            } else {
                u = (code & 0xF) << 4;
                v = (((code - 0x8240) >> 4) << 4) + 72;
            }
        } else {
            s32 tbl[30] = {
                0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166,
                0x8169, 0x816A, 0x8196, 0x817B, 0x8143, 0x817C, 0x8144,
                0x815E, 0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148,
                0x8140, 0x83BF, 0x83C0, 0x81C1, 0x81A9, 0x81A8, 0x81BC,
                0x81BD, -1,
            };
            i = 0;
            for (;;) {
                if (code == tbl[i]) {
                    if (i < 15) {
                        u = i << 4;
                        v = 72;
                    } else if (i < 22) {
                        u = (i << 4) - 96;
                        v = 88;
                    } else {
                        switch (code) {
                        case 0x83BF: u = 208; v = 152; break;
                        case 0x83C0: u = 224; v = 152; break;
                        case 0x81C1: u = 240; v = 152; break;
                        case 0x81A9: u = 176; v = 120; break;
                        case 0x81A8: u = 192; v = 120; break;
                        case 0x81BC: u = 224; v = 120; shift = 2; break;
                        case 0x81BD: u = 240; v = 120; shift = -2; break;
                        }
                    }
                    goto draw;
                }
                if (tbl[i] < 0) {
                    break;
                }
                i++;
            }
            u = 0;
            v = 120;
        }
draw:
        DisplayObject_ConfigureScreenSprite(
            obj,
            p->rect_x + w->x_0C + shift, p->rect_y + w->y_0E,
            16, 16, u, v, 10, 640, p->color_index_54 + 232
        );
        obj->sequence = gNameEntry_bGlyphSequence;
        gNameEntry_bGlyphSequence = gNameEntry_bGlyphSequence + 1;
    } else {
        DisplayObject_ConfigureScreenSprite(
            obj,
            gNameEntry_pSelectionFrame->x, gNameEntry_pSelectionFrame->y, 16, 16, 128, 128, 23,
            256, 240
        );
    }
    obj->field_48 = 0x80008;
    obj->textBoxSlot = slot;
    obj->sourceGlyph = w;
    return obj;
}
#endif
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_ADJUST_LENGTH) || \
    defined(VERSION_EUROPE_PASSWORD_ADJUST_LENGTH)
s32 NameEntry_AdjustLength(s32 delta, s32 arg)
{
    DisplayObject *object;

    if (delta < 0) {
        if (gNameEntry_bCaretPos == 0) {
            return 0;
        }
    } else {
        if (gNameEntry_bCaretPos == 5) {
            return 0;
        }
    }
    gNameEntry_bCaretPos += delta;
    object = (DisplayObject *)gNameEntry_pCaret;
    object->field_44.h.field_44 = gNameEntry_bCaretPos * 16 + 0x6B;
    object->update = (DisplayObjectCallback)NameEntry_UpdateCaretTween;
    object->field_6C = 2;
    object->field_60 = arg;
    object->field_44.h.field_46 = object->field_30.h.field_32;
    SD_SEPlayFull(0xC);
    return 1;
}
#endif

#if (!defined(VERSION_JAPAN) && !defined(VERSION_EUROPE)) || \
    defined(VERSION_JAPAN_PASSWORD_UPDATE_KEYBOARD) || \
    defined(VERSION_EUROPE_PASSWORD_UPDATE_KEYBOARD)
#ifndef NAME_ENTRY_CONFIRM_MASK
#define NAME_ENTRY_CONFIRM_MASK PAD_BUTTON_CONFIRM_MASK
#endif
#ifndef NAME_ENTRY_CANCEL_MASK
#define NAME_ENTRY_CANCEL_MASK PAD_BUTTON_CANCEL
#endif

static __inline__ s32 NameEntry_GetKeyboardCellCode(s8 *table, s32 row, s32 column)
{
    column += row * 15;
    return table[column] & 0xF;
}

void NameEntry_UpdateKeyboard(void)
{
#ifdef VERSION_EUROPE
    s32 work;
    SelectionFrame *w;
    DisplayObject *object;
#else
    SelectionFrame *w;
    s32 work;
#endif
    s8 *glyphTable;
    s32 glyphRow;
    s32 glyphCol;
    s32 glyphCode;
    s32 home;
#ifndef VERSION_EUROPE
    s32 n;
#endif
    s32 walkCol;
    s32 col;
    s32 row;
    DuelEffectEntry *node;
#ifndef VERSION_EUROPE
    u8 *obj;
#endif
    s32 kind;
#ifdef VERSION_EUROPE
    s32 n;
#endif
    s32 gx;
    s32 gy;
    s32 d;

    w = gNameEntry_pSelectionFrame;
    if ((gNameEntry_wSelectionFrameFlags & 0x4000) != 0) {
        home = w->width - 16;
        work = w->widthBonus - home;
        if (work != 0) {
            w->width = (work >= 0) ? (w->width + 2) : (w->width - 2);
        }
        DisplayObject_StepPositionXY((DisplayObjectVelocity *)w);
        w->timer = w->timer - 1;
        if (w->timer != 0) {
            return;
        }
        w->width = w->widthBonus + 16;
        w->x = gNameEntry_wCursorTargetX;
        w->y = gNameEntry_wCursorTargetY;
        gNameEntry_wSelectionFrameFlags &= 0xBFFF;
    }
    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            gNameEntry_bKeyboardColumn = gNameEntry_bKeyboardColumn + 1;
            if (gNameEntry_bKeyboardColumn >= 15) {
                gNameEntry_bKeyboardColumn = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            gNameEntry_bKeyboardColumn = gNameEntry_bKeyboardColumn - 1;
            if (gNameEntry_bKeyboardColumn < 0) {
                gNameEntry_bKeyboardColumn = 14;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if (gNameEntry_bKeyboardColumn >= 11) {
#ifdef VERSION_EUROPE
                SD_SEPlayFull(9);
                return;
#else
                n = gInput_wPad1Held & PAD_DIRECTION_DOWN;
                work = (n != 0);
                gNameEntry_bKeyboardColumn = 11;
                gNameEntry_bSavedRow = gNameEntry_bKeyboardRow = gNameEntry_aRowLinks[(s8)gNameEntry_bKeyboardRow][work];
                goto tail47;
#endif
            } else if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
                gNameEntry_bKeyboardRow = gNameEntry_bKeyboardRow - 1;
                if ((s8)gNameEntry_bKeyboardRow < 0) {
                    gNameEntry_bKeyboardRow = 8;
                }
            } else {
                gNameEntry_bKeyboardRow = gNameEntry_bKeyboardRow + 1;
                if ((s8)gNameEntry_bKeyboardRow >= 9) {
                    gNameEntry_bKeyboardRow = 0;
                }
            }
            gNameEntry_bSavedRow = gNameEntry_bKeyboardRow;
        }
    } else {
        goto alt800;
    }
tail47:
    SD_SEPlayFull(47);
#ifdef VERSION_EUROPE
    {
        s32 tableBase;

        tableBase = (s32)&gNameEntry_aKeyboardCells[0][0];
        do {
        } while (0);
        row = (s8)gNameEntry_bKeyboardRow;
        {
            PasswordGlyphCoordinates coords = {
                parts: { (s8)gNameEntry_bKeyboardColumn, row * 15 }
            };
            col = coords.parts.column;
            work = *(s8 *)((coords.parts.column + coords.parts.stride) +
                           tableBase);
            walkCol = col;
            while (work < 0) {
                /* Preserve GCC's separate loop-invariant address lifetimes. */
                do {
                    walkCol = walkCol + work;
                    gNameEntry_bKeyboardColumn = walkCol;
                    work = *(s8 *)(((s8)walkCol + coords.parts.stride) +
                                   tableBase);
                } while (0);
            }
        }
    }
#else
    obj = (u8 *)&gNameEntry_aKeyboardCells[0][0];
    row = (s8)gNameEntry_bKeyboardRow;
    {
        /* Keep the pair fully initialized; separate stores change allocation. */
        PasswordGlyphCoordinates coords = {
            parts: { (s8)gNameEntry_bKeyboardColumn, row * 15 }
        };
        col = coords.parts.column;
        work = *(s8 *)((coords.parts.column + coords.parts.stride) + (s32)obj);
        walkCol = col;
        while (work < 0) {
            walkCol = walkCol + work;
            gNameEntry_bKeyboardColumn = walkCol;
            work = *(s8 *)(((s8)walkCol + coords.parts.stride) + (s32)obj);
        }
    }
#endif
    gNameEntry_bKeyboardRow = gNameEntry_bSavedRow;
    w->widthBonus = 0;
    if (work != 0) {
        if ((work & 0x40) != 0) {
            w->widthBonus = 20;
        }
        gNameEntry_bKeyboardRow = work & 0xF;
    }
    gNameEntry_wCursorTargetX = (s8)gNameEntry_bKeyboardColumn * 20 + 22;
    if ((s8)gNameEntry_bKeyboardColumn >= 11) {
        gNameEntry_wCursorTargetX = (s8)gNameEntry_bKeyboardColumn * 20 + 42;
    }
    gNameEntry_wCursorTargetY = (s8)gNameEntry_bKeyboardRow * 18 + 24;
    DisplayObject_ResetVelocity((DisplayObjectVelocity *)w);
    w->timer = 8;
    w->stepX = ((gNameEntry_wCursorTargetX - w->x) << 8) / 8;
    w->stepY = ((gNameEntry_wCursorTargetY - w->y) << 8) / 8;
    gNameEntry_wSelectionFrameFlags |= 0x4000;
    return;

alt800:
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto select;
    }
    gNameEntry_bKeyboardColumn = 14;
    gNameEntry_bKeyboardRow = 8;
    goto tail47;
select:
#ifdef VERSION_EUROPE
    if ((gInput_wPad1Repeat & 0xC) != 0) {
        work = 1;
        if ((gInput_wPad1Repeat & 4) != 0) {
            work = -1;
        }
        if (NameEntry_AdjustLength(work, 6) == 0) {
            SD_SEPlayFull(9);
        }
        return;
    }
#endif
    if ((gInput_wPad1Repeat & NAME_ENTRY_CONFIRM_MASK) == 0) {
        goto sel_ret;
    }
#ifdef VERSION_EUROPE
    kind = 0;
    work = -1;
    gx = 0;
    gy = 0;
    glyphTable = &gNameEntry_aKeyboardCells[0][0];
    glyphRow = (s8)gNameEntry_bKeyboardRow;
    glyphCol = col = (s8)gNameEntry_bKeyboardColumn;
    glyphCode = NameEntry_GetKeyboardCellCode(glyphTable, glyphRow, glyphCol);
    if (glyphCode == 8) {
        work = 2;
        gy = 144;
        gNameEntry_bStateFlags |= 0x40;
    } else {
        n = 41;
        kind = 1;
        gx = glyphCol * 20;
        gy = glyphRow * 18;
        if (glyphRow == 8 && (u8)(gNameEntry_bKeyboardColumn - 9) < 2) {
            work = glyphCol - 9;
            d = kind;
            if (work == 0) {
                d = -1;
            }
            if (NameEntry_AdjustLength(d, 6) == 0) {
                n = 9;
            }
        }
        SD_SEPlayFull(n);
    }
    if (work >= 0) {
        object = gNameEntry_apKeySprites[work];
        object->field_6C = 1;
        object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
        object->field_4C = 0;
        object->field_6B = 1;
        return;
    }
    node = TextBox_GetGlyphAt(kind, gx | 2, gy);
    object = NameEntry_SpawnGlyphSprite(kind, node);
    object->field_6C = 1;
    object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        object->flags &= 0xFFBF;
    }
    if (kind == 1) {
        u16 *slot;
        gNameEntry_bStateFlags |= 0x80;
        slot = (u16 *)(gNameEntry_bCaretPos * 2 + (s32)gNameEntry_pNameBuffer);
        *slot = 0;
        if (node != 0) {
            *slot = node->code_00;
        }
        object = NameEntry_SpawnGlyphSprite(1, node);
        object->field_60 = 8;
        object->update = (DisplayObjectCallback)NameEntry_UpdateGlyphTransfer;
        object->field_44.h.field_46 = 204;
        object->field_44.h.field_44 = gNameEntry_bCaretPos * 16 + 114;
        object->field_6C = 6;
    }
    return;
#else
    kind = 0;
    work = kind;
#ifdef VERSION_JAPAN
    gy = kind;
#endif
    glyphTable = &gNameEntry_aKeyboardCells[0][0];
    glyphRow = (s8)gNameEntry_bKeyboardRow;
    glyphCol = col = (s8)gNameEntry_bKeyboardColumn;
    glyphCode = NameEntry_GetKeyboardCellCode(glyphTable, glyphRow, glyphCol);
    gx = kind;
#ifdef VERSION_JAPAN
    switch (glyphCode) {
    case 2:
        goto arm2;
    case 4:
        goto arm4;
    case 6:
        goto arm6;
    }
    goto arme;
arm2:
    /* The Japanese keyboard's mode key: it cycles the character set
     * gNameEntry_bCharacterSet through 0, 1 and 2 and rebuilds the keyboard for it. */
    gNameEntry_bCharacterSet++;
    if (gNameEntry_bCharacterSet >= 3) {
        gNameEntry_bCharacterSet = 0;
    }
    NameEntry_BuildKeyboardTextBox(gNameEntry_bCharacterSet);
    work = 2;
    SD_SEPlayFull(7);
    goto join;
#else
    if (glyphCode == 4) {
        goto arm4;
    }
    if (glyphCode == 6) {
        goto arm6;
    }
    goto arme;
#endif
arm4:
    d = 1;
    if (glyphCol == 11) {
        d = -1;
    } else {
        gx = 20;
    }
    if (NameEntry_AdjustLength(d, 6) == 0) {
        SD_SEPlayFull(9);
    }
    gy = 36;
    goto join;
arm6:
    work = 2;
    gy = 72;
    gNameEntry_bStateFlags |= 0x40;
    goto join;
arme:
    kind = 1;
    gx = (s8)gNameEntry_bKeyboardColumn * 20;
    gy = ((s8)gNameEntry_bKeyboardRow * 9) << kind;
    SD_SEPlayFull(41);
join:
    ;
    node = TextBox_GetGlyphAt(kind, gx, gy);
    obj = NameEntry_SpawnGlyphSprite(kind, node);
    ((DisplayObject *)obj)->field_6C = 1;
    ((DisplayObject *)obj)->update =
        (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        ((DisplayObject *)obj)->flags &= 0xFFBF;
    }
    if (work != 0) {
        ((DisplayObject *)obj)->field_48.h.field_48 = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = NameEntry_SpawnGlyphSprite(kind, node);
        ((DisplayObject *)obj)->field_6C = 1;
        ((DisplayObject *)obj)->update =
            (DisplayObjectCallback)NameEntry_UpdateGlyphPulse;
        ((DisplayObject *)obj)->field_48.h.field_48 = 0;
    }
    if (kind == 1) {
        u16 *slot;
        gNameEntry_bStateFlags |= 0x80;
        slot = (u16 *)(gNameEntry_bCaretPos * 2 + (s32)gNameEntry_pNameBuffer);
        *slot = 0;
        if (node != 0) {
            *slot = node->code_00;
        }
        obj = NameEntry_SpawnGlyphSprite(1, node);
        ((DisplayObject *)obj)->field_60 = 8;
        ((DisplayObject *)obj)->update =
            (DisplayObjectCallback)NameEntry_UpdateGlyphTransfer;
        ((DisplayObject *)obj)->field_44.h.field_46 = 204;
        ((DisplayObject *)obj)->field_44.h.field_44 =
            gNameEntry_bCaretPos * 16 + 112;
        ((DisplayObject *)obj)->field_6C = 6;
    }
    return;
#endif
sel_ret:
#ifdef VERSION_EUROPE
    if ((gInput_wPad1Repeat & NAME_ENTRY_CANCEL_MASK) != 0) {
        if (NameEntry_AdjustLength(-1, 6) != 0) {
            SD_SEPlayFull(12);
        } else {
            SD_SEPlayFull(9);
        }
    }
#else
    if ((gInput_wPad1Repeat & NAME_ENTRY_CANCEL_MASK) != 0) {
        n = NameEntry_AdjustLength(-1, 6);
        if (n != 0) {
            n = 12;
        } else {
            n = 9;
        }
        SD_SEPlayFull(n);
    }
#endif
    return;
}
#endif
