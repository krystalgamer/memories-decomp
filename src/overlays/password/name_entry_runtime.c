#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/input.h"
#include "../../game/display_object_config.h"
#include "../../game/text_constants.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "../../game/display_object_lifecycle.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"
#include "../../game/func_80039794.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/func_80043178.h"
#include "../../game/gpu_packets.h"
#include "../../game/sound.h"
#include "../../game/save_data.h"
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

/* The name-entry screen pipeline in executable order, up to the keyboard:
   setup and selection-frame drawing, then glyph lookup and effects. The
   eleven functions share one compiler profile and the D_8016D400 state
   block; the leading display-object helper has no proven ownership in this
   lifecycle. The keyboard handler that follows, NameEntry_UpdateKeyboard,
   matched only through pinned registers and an asm statement and is a stored
   candidate since #3859 (src/candidates/password/func_8016913C.c); the
   dialog and completion handling after it are in name_entry_dialog.c. */

extern u8 D_8016D403;
extern u8 D_8016D4D0;

void NameEntry_BuildKeyboardTextBox(s32 textOffset)
{
    DuelEffectChannel *object;

    func_8003B6AC(1, 1);
    object = TextBox_Create(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0);
    object->field_5A = 0x14;
    object->field_5B = 0x12;
    func_80039A14((struct DuelEffectChannel *)object);
}

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

void NameEntry_Init(void)
{
    u8 *obj;
    DuelEffectChannel *boxes;
    DuelEffectChannel *sprite;

    SD_BGMPlay(28688);
    func_800403F0();
    D_8016D400 = 0;
    D_8016D403 = 0;
    D_8016D408 = 0;
    D_8016D418 = gSaveData_aPlayerNameSjis;
    Text_SjisToGlyphCodes(D_801B125A, gSaveData_aPlayerNameSjis,
                  SAVE_DATA_PLAYER_NAME_CHAR_COUNT);
    func_8003B6AC(3, 1);
    TextBox_Create(3, 254, 112, 204, 96, 16);
    boxes = D_800EB0F8;
    boxes[3].field_5A = 16;
    boxes[3].field_5B = 16;
    func_80039A14((struct DuelEffectChannel *)&boxes[3]);
    func_8003B6AC(0, 1);
    sprite = TextBox_Create(0, 243, 262, 60, 100, 100);
    sprite->field_5A = 20;
    sprite->field_5B = 18;
    func_80039A14((struct DuelEffectChannel *)sprite);
    D_8016D4D0 = 2;
    NameEntry_BuildKeyboardTextBox(2);
    D_8016D426 = 0;
    D_8016D402 = 0;
    D_8016D401 = 0;
    D_8016D42C = 0;
    obj = func_800400AC(func_8004002C(), 6);
    D_8016D434 = 22;
    *(s16 *)(obj + 0x30) = 22;
    D_8016D436 = 24;
    *(s16 *)(obj + 0x32) = 24;
    *(s16 *)(obj + 0x3C) = 16;
    *(s16 *)(obj + 0x3E) = 16;
    func_80042918(obj);
    func_800428EC(obj, 10);
    *(void **)(obj + 0x4C) = NameEntry_DrawSelectionFrame;
    D_8016D404 = (SelectionFrame *)obj;
    obj = func_800400AC(func_8004002C(), 1);
    func_80040510((DisplayObjectConfigView *)obj, 107, 199, 32, 32, 144, 128, 23, 256, 240);
    *(s16 *)(obj + 0x4A) = 13;
    *(s16 *)(obj + 0x48) = 13;
    *(s32 *)(obj + 4) = *(s32 *)(obj + 4) | GsALON;
    func_80042918(obj);
    func_800428EC(obj, 10);
    D_8016D43C = obj;
    obj = func_800400AC(func_8004002C(), 3);
    func_80040510((DisplayObjectConfigView *)obj, 0, 0, 320, 240, 0, 0, 20, 256, 243);
    *(s32 *)(obj + 0xC) = 0x404040;
    *(s32 *)(obj + 4) = *(s32 *)(obj + 4) | 0x1000000;
    func_8004293C(obj);
    D_8016D4D2 = 244;
    Fade_WaitIn();
}

DuelEffectEntry *TextBox_GetGlyphAt(s32 index, s32 x, s32 y)
{
    DuelEffectChannel *base;
    DuelEffectEntry *node;

    base = D_800EB0F8;
    node = base[index].entry_head_24;
    for (;;) {
        if (!(node->flags_11 & 0x80)) {
            return (DuelEffectEntry *)0;
        }
        if (node->x_0C == x && node->y_0E == y) {
            return node;
        }
        node++;
    }
}

/* One frame of the flash a glyph makes when it is picked on the keyboard: the
 * source node is parked off-screen for the twelve frames the sprite scales up
 * and back down, then restored. */
void NameEntry_UpdateGlyphPulse(u8 *sprite)
{
    GlyphSprite *obj = (GlyphSprite *)sprite;
    DuelEffectEntry *source;
    s32 step;
    s32 value;
    s16 frame;

    source = obj->sourceGlyph;
    if (func_80042B98((DisplayObjectLifecycle *)sprite) == 0) {
        if (source != 0) {
            obj->savedSourceX = source->x_0C;
            source->x_0C = 0x400;
        }
        obj->frame = 0;
        obj->flags &= ~GsROTOFF;
    }
    value = obj->frame;
    if (value >= 12) {
        func_8004036C(obj);
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
            source->x_0C = obj->savedSourceX;
        }
    }
}

/* One frame of a single shard: seeded once with a random sideways kick and an
 * upward one, then thrown until it falls off the bottom of the screen. */
void NameEntry_UpdateGlyphFragment(u8 *object)
{
    u8 flags;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        DisplayObject_ResetVelocity(object);
        *(s16 *)(object + 0x36) = Rand_GetInterval(0x200) - 0x100;
        *(s16 *)(object + 0x38) = -Rand_GetInterval(0x180);
    }
    *(s16 *)(object + 0x36) =
        DisplayObject_StepTowardZero(*(s16 *)(object + 0x36), 8);
    *(s16 *)(object + 0x38) =
        DisplayObject_StepToward(*(s16 *)(object + 0x38), 0x800, 0x40);
    DisplayObject_StepPositionXY(object);
    if (*(s16 *)(object + 0x32) >= 0xF0) {
        func_8004036C(object);
    }
}

/* Breaks one 16x16 glyph into sixteen 4x4 shards, each on
 * NameEntry_UpdateGlyphFragment, and retires the glyph itself. */
void NameEntry_UpdateGlyphShatter(u8 *object)
{
    u8 *piece;
    s32 dx;
    s32 dy;

    if (func_80042B98((DisplayObjectLifecycle *)object) == 0) {
        for (dy = 0; dy < 0x10; dy += 4) {
            for (dx = 0; dx < 0x10; dx += 4) {
                piece = func_800400AC(func_8004002C(), 1);
                if (piece != 0) {
                    func_80040510((DisplayObjectConfigView *)piece,
                                  *(s16 *)(object + 0x30) + dx,
                                  *(s16 *)(object + 0x32) + dy,
                                  4, 4,
                                  object[0x5C] + dx,
                                  object[0x5D] + dy,
                                  object[0x66],
                                  *(u16 *)(object + 0x40),
                                  *(u16 *)(object + 0x42));
                    piece[0x6C] = 3;
                    *(NameEntryGlyphUpdate *)(piece + 0x24) =
                        NameEntry_UpdateGlyphFragment;
                }
            }
        }
    } else {
        func_8004036C(object);
    }
}

/* Slides the caret to +0x44/+0x46 over the +0x60 frames it was given, then
 * snaps to the target and uninstalls itself. */
void NameEntry_UpdateCaretTween(u8 *object)
{
    u8 flags;
    s16 remaining;

    flags = object[0x6C];
    if ((flags & 0x80) == 0) {
        object[0x6C] = flags | 0x80;
        DisplayObject_ResetVelocity(object);
        *(s16 *)(object + 0x36) =
            ((*(s16 *)(object + 0x44) - *(s16 *)(object + 0x30)) << 8) /
            *(s16 *)(object + 0x60);
        *(s16 *)(object + 0x38) =
            ((*(s16 *)(object + 0x46) - *(s16 *)(object + 0x32)) << 8) /
            *(s16 *)(object + 0x60);
    }
    DisplayObject_StepPositionXY(object);
    remaining = *(u16 *)(object + 0x60) - 1;
    *(s16 *)(object + 0x60) = remaining;
    if (remaining <= 0) {
        *(s32 *)(object + 0x24) = 0;
        object[0x6C] = 0;
        *(s32 *)(object + 0x30) = *(s32 *)(object + 0x44);
    }
}

/* Flies an accepted glyph from the keyboard to the name field on the same
 * tween prologue as the caret, dropping a fading copy of itself each frame.
 * On arrival it looks the destination glyph up and shatters it. */
void NameEntry_UpdateGlyphTransfer(u8 *w)
{
    u8 *o;
    DuelEffectEntry *node;
    s32 n;

    if ((w[0x6C] & 0x80) == 0) {
        w[0x6C] |= 0x80;
        DisplayObject_ResetVelocity(w);
        *(s16 *)(w + 0x36) =
            ((*(s16 *)(w + 0x44) - *(s16 *)(w + 0x30)) << 8) /
            *(s16 *)(w + 0x60);
        *(s16 *)(w + 0x38) =
            ((*(s16 *)(w + 0x46) - *(s16 *)(w + 0x32)) << 8) /
            *(s16 *)(w + 0x60);
    }
    if ((w[0x6C] & 0x40) != 0) {
        n = *(u16 *)(w + 0x60) - 1;
        *(s16 *)(w + 0x60) = n;
        if ((s16)n <= 0) {
            func_8004036C(w);
        }
        return;
    }
    DisplayObject_StepPositionXY(w);
    n = *(u16 *)(w + 0x60) - 1;
    *(s16 *)(w + 0x60) = n;
    if ((s16)n > 0) {
        o = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o, *(s16 *)(w + 0x30), *(s16 *)(w + 0x32), 16, 16,
                      w[0x5C], w[0x5D], w[0x66], *(u16 *)(w + 0x40),
                      *(u16 *)(w + 0x42));
        *(u32 *)(o + 0x0C) = 0x606060;
        *(s16 *)(o + 0x60) = 6;
        *(void **)(o + 0x24) = func_80042BC0;
        *(u32 *)(o + 4) |= (GsALON | GsAONE);
        return;
    }
    w[0x6C] |= 0x40;
    *(u32 *)(w + 0x30) = *(u32 *)(w + 0x44);
    /* Keep the timer reload inside the final argument; its expression
       placement controls the retail argument-setup order. */
    node = TextBox_GetGlyphAt(3, D_8016D42C << 4, (*(s16 *)(w + 0x60) = 2, 0));
    if (node == 0) {
        return;
    }
    o = NameEntry_SpawnGlyphSprite(3, node);
    *(NameEntryGlyphUpdate *)(o + 0x24) = NameEntry_UpdateGlyphShatter;
    o[0x6C] = 5;
}

/* Makes the sprite for one glyph node: maps the node's Shift-JIS code to a
 * cell in the 16x16 font page, places it at the text box's origin plus the
 * node's local position, and stamps the spawn order into +0x6A. With no node
 * it makes the cursor sprite at D_8016D404's position instead. */
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
    obj = func_800400AC(func_8004002C(), 1);
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
        func_80040510(
            (DisplayObjectConfigView *)obj,
            p->field_3C + w->x_0C + shift, p->field_40 + w->y_0E,
            16, 16, u, v, 10, 640, p->field_54 + 232
        );
        obj->sequence = D_8016D408;
        D_8016D408 = D_8016D408 + 1;
    } else {
        func_80040510(
            (DisplayObjectConfigView *)obj,
            D_8016D404->x, D_8016D404->y, 16, 16, 128, 128, 23,
            256, 240
        );
    }
    obj->field_48 = 0x80008;
    obj->textBoxSlot = slot;
    obj->sourceGlyph = w;
    return obj;
}

s32 NameEntry_AdjustLength(s32 delta, s32 arg)
{
    u8 *object;

    if (delta < 0) {
        if (D_8016D42C == 0) {
            return 0;
        }
    } else {
        if (D_8016D42C == 5) {
            return 0;
        }
    }
    D_8016D42C += delta;
    object = D_8016D43C;
    *(s16 *)(object + 0x44) = D_8016D42C * 16 + 0x6B;
    *(void **)(object + 0x24) = NameEntry_UpdateCaretTween;
    object[0x6C] = 2;
    *(s16 *)(object + 0x60) = arg;
    *(s16 *)(object + 0x46) = *(u16 *)(object + 0x32);
    SD_SEPlayFull(0xC);
    return 1;
}
