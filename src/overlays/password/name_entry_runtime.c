#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
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

/* The complete name-entry screen pipeline in executable order: setup and
   selection-frame drawing, glyph lookup and effects, then keyboard, dialog,
   and completion handling. The fourteen functions share one compiler profile
   and the D_8016D400 state block; the leading display-object helper and the
   following shop display unit have no proven ownership in this lifecycle. */

extern u16 D_8016D4D4;
extern s8 D_8016AB38[][15];
extern u8 D_8016ABC0[][2];
extern u8 D_8016D403;
extern u8 D_8016D4D0;
extern u8 gSaveData_aPlayerNameSjis[];
extern DuelEffectChannel D_800EB1C0;
extern u8 D_8016D41C;

void NameEntry_BuildKeyboardTextBox(s32 textOffset)
{
    DuelEffectChannel *object;

    func_8003B6AC(1, 1);
    object = TextBox_Create(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0);
    object->field_5A = 0x14;
    object->field_5B = 0x12;
    func_80039A14((u8 *)object);
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
    func_80039A14((u8 *)&boxes[3]);
    func_8003B6AC(0, 1);
    sprite = TextBox_Create(0, 243, 262, 60, 100, 100);
    sprite->field_5A = 20;
    sprite->field_5B = 18;
    func_80039A14(sprite);
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

void NameEntry_UpdateKeyboard(void)
{
    SelectionFrame *w;
    s32 work;
    register s8 *glyphTable __asm__("$4");
    s32 glyphRow;
    s32 glyphCol;
    register s32 glyphIndex __asm__("$2");
    s32 glyphCode;
    s32 home;
    s32 n;
    register s32 walkCol __asm__("$3");
    s32 col;
    s32 row;
    DuelEffectEntry *node;
    u8 *obj;
    s32 kind;
    s32 gx;
    s32 gy;
    s32 d;
    /* Keep this coordinate pair in a0/a1, leaving the global's high half in a2. */
    register union {
        u64 all;
        struct {
            s32 col;
            s32 stride;
        } parts;
    } coords __asm__("$4");

    w = D_8016D404;
    if ((D_8016D4D4 & 0x4000) != 0) {
        home = w->width - 16;
        work = w->widthBonus - home;
        if (work != 0) {
            w->width = (work >= 0) ? (w->width + 2) : (w->width - 2);
        }
        DisplayObject_StepPositionXY(w);
        w->timer = w->timer - 1;
        if (w->timer != 0) {
            return;
        }
        w->width = w->widthBonus + 16;
        w->x = D_8016D434;
        w->y = D_8016D436;
        D_8016D4D4 &= 0xBFFF;
    }
    if ((gInput_wPad1Held & PAD_DIRECTION_MASK) != 0) {
        if ((gInput_wPad1Held & PAD_DIRECTION_RIGHT) != 0) {
            D_8016D401 = D_8016D401 + 1;
            if (D_8016D401 >= 15) {
                D_8016D401 = 0;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_LEFT) != 0) {
            D_8016D401 = D_8016D401 - 1;
            if (D_8016D401 < 0) {
                D_8016D401 = 14;
            }
        }
        if ((gInput_wPad1Held & PAD_DIRECTION_VERTICAL_MASK) != 0) {
            if (D_8016D401 >= 11) {
                n = gInput_wPad1Held & PAD_DIRECTION_DOWN;
                work = (n != 0);
                D_8016D401 = 11;
                D_8016D426 = D_8016D402 = D_8016ABC0[(s8)D_8016D402][work];
                goto tail47;
            } else if ((gInput_wPad1Held & PAD_DIRECTION_UP) != 0) {
                D_8016D402 = D_8016D402 - 1;
                if ((s8)D_8016D402 < 0) {
                    D_8016D402 = 8;
                }
            } else {
                D_8016D402 = D_8016D402 + 1;
                if ((s8)D_8016D402 >= 9) {
                    D_8016D402 = 0;
                }
            }
            D_8016D426 = D_8016D402;
        }
    } else {
        goto alt800;
    }
tail47:
    SD_SEPlayFull(47);
    obj = (u8 *)&D_8016AB38[0][0];
    row = (s8)D_8016D402;
    coords.parts.col = (s8)D_8016D401;
    coords.parts.stride = row * 15;
    col = coords.parts.col;
    work = *(s8 *)((coords.parts.col + coords.parts.stride) + (s32)obj);
    walkCol = col;
    while (work < 0) {
        walkCol = walkCol + work;
        D_8016D401 = walkCol;
        work = *(s8 *)(((s8)walkCol + coords.parts.stride) + (s32)obj);
    }
    D_8016D402 = D_8016D426;
    w->widthBonus = 0;
    if (work != 0) {
        if ((work & 0x40) != 0) {
            w->widthBonus = 20;
        }
        D_8016D402 = work & 0xF;
    }
    D_8016D434 = (s8)D_8016D401 * 20 + 22;
    if ((s8)D_8016D401 >= 11) {
        D_8016D434 = (s8)D_8016D401 * 20 + 42;
    }
    D_8016D436 = (s8)D_8016D402 * 18 + 24;
    DisplayObject_ResetVelocity(w);
    w->timer = 8;
    w->stepX = ((D_8016D434 - w->x) << 8) / 8;
    w->stepY = ((D_8016D436 - w->y) << 8) / 8;
    D_8016D4D4 |= 0x4000;
    return;

alt800:
    if ((gInput_wPad1Pressed & PAD_BUTTON_START) == 0) {
        goto select;
    }
    D_8016D401 = 14;
    D_8016D402 = 8;
    goto tail47;
select:
    if ((gInput_wPad1Repeat & PAD_BUTTON_CONFIRM_MASK) == 0) {
        goto sel_ret;
    }
    kind = 0;
    work = kind;
    glyphTable = &D_8016AB38[0][0];
    glyphRow = (s8)D_8016D402;
    glyphCol = col = (s8)D_8016D401;
    glyphIndex = glyphRow * 15;
    glyphIndex = glyphCol + glyphIndex;
    glyphCode = glyphTable[glyphIndex] & 0xF;
    gx = kind;
    if (glyphCode == 4) {
        goto arm4;
    }
    if (glyphCode == 6) {
        goto arm6;
    }
    goto arme;
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
    D_8016D400 |= 0x40;
    goto join;
arme:
    kind = 1;
    gx = (s8)D_8016D401 * 20;
    gy = ((s8)D_8016D402 * 9) << kind;
    SD_SEPlayFull(41);
join:
    ;
    node = TextBox_GetGlyphAt(kind, gx, gy);
    obj = NameEntry_SpawnGlyphSprite(kind, node);
    obj[0x6C] = 1;
    *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphPulse;
    if (node == 0) {
        *(u16 *)(obj + 8) &= 0xFFBF;
    }
    if (work != 0) {
        *(s16 *)(obj + 0x48) = 20;
        node = TextBox_GetGlyphAt(kind, gx + 20, gy);
        obj = NameEntry_SpawnGlyphSprite(kind, node);
        obj[0x6C] = 1;
        *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphPulse;
        *(s16 *)(obj + 0x48) = 0;
    }
    if (kind == 1) {
        u16 *slot;
        D_8016D400 |= 0x80;
        slot = (u16 *)(D_8016D42C * 2 + (s32)D_8016D418);
        *slot = 0;
        if (node != 0) {
            *slot = node->code_00;
        }
        obj = NameEntry_SpawnGlyphSprite(1, node);
        *(s16 *)(obj + 0x60) = 8;
        *(NameEntryGlyphUpdate *)(obj + 0x24) = NameEntry_UpdateGlyphTransfer;
        *(s16 *)(obj + 0x46) = 204;
        *(s16 *)(obj + 0x44) = D_8016D42C * 16 + 112;
        obj[0x6C] = 6;
    }
    return;
sel_ret:
    if ((gInput_wPad1Repeat & PAD_BUTTON_CANCEL) != 0) {
        n = NameEntry_AdjustLength(-1, 6);
        if (n != 0) {
            n = 12;
        } else {
            n = 9;
        }
        SD_SEPlayFull(n);
    }
    return;
}

void NameEntry_UpdateDialog(void)
{
    DuelEffectChannel *box;
    DialogCaret *caret;
    u8 flags;
    u8 fb;
    u8 fc;
    u8 fd;
    u8 fe;
    u8 ff;
    u8 fg;
    s32 id;
    s16 pos;
    u8 *p;
    u8 *next;
    DuelEffectChannel *panel;
    s32 c;
    s32 term;

    if (D_8016D4D2 != 0) {
        flags = D_8016D400;
        if ((flags & 4) == 0) {
            D_8016D400 = flags | 4;
            func_8003B6AC(2, 2);
            box = TextBox_Create(2, D_8016D4D2 & 0xFFF, 16, 248,
                                 288, 48);
            box->field_59 = 20;
            id = D_8016D4D2;
            if ((id & 0x8000) == 0) {
                if ((id & 0x4000) == 0) {
                    func_80039A14((u8 *)box);
                    D_8009B34D = 0;
                }
                box->field_30 = Dialog_OpenChoice(box);
            } else {
                box->flags_34 |= 8;
                do {
                    func_80039794();
                } while (box->field_30 == 0);
            }
            caret = func_800400AC(func_8004002C(), 2);
            func_800404CC(caret, 16, 248, 0, 0, 0, 23, 257);
            func_80042918(caret);
            func_800428EC(caret, 19);
            caret->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
            box->field_2C = caret;
            func_80043178(caret);
            caret->slide = -1024;
            D_8016D400 |= 2;
            return;
        }
        box = &D_800EB1C0;
        caret = (DialogCaret *)box->field_2C;
        if ((flags & 2) != 0) {
            pos = caret->slide;
            if (pos >= 0) {
                caret->slide = pos - 85;
                func_80043230((DisplayObjectPosition *)caret, 16, 248,
                              (s16)(pos - 85));
                if (caret->slide < 0) {
                    caret->x = 16;
                    fb = D_8016D400;
                    caret->y = 248;
                    D_8016D400 = fb & 0xF9;
                    TextBox_Destroy(box);
                    D_8016D4D2 = 0;
                    return;
                }
            } else {
                caret->slide = pos + 85;
                func_80043230((DisplayObjectPosition *)caret, 16, 176,
                              (s16)(pos + 85));
                if (caret->slide >= 0) {
                    caret->x = 16;
                    fc = D_8016D400;
                    caret->y = 176;
                    D_8016D400 = fc & 0xFD;
                }
            }
            TextBox_SetPos((u8 *)box, caret->x, caret->y);
            return;
        }
        if ((D_8016D4D2 & 0x8000) == 0) {
            if ((gInput_wPad1Pressed & (PAD_BUTTON_CANCEL | PAD_BUTTON_CONFIRM_MASK)) == 0) {
                SD_SEPlayFull(11);
                return;
            }
        } else {
            func_80039794();
            if ((*(u32 *)&box->flags_34 & 0x2008) != 0x2000) {
                return;
            }
        }
        if (D_8009B34D != 0) {
            fd = D_8016D400;
            D_8016D400 = fd & 0xDF;
        }
        if ((D_8016D400 & 0x20) != 0) {
            D_8016D4D2 = 0;
        }
        func_80043178(caret);
        fe = D_8016D400;
        caret->slide = 1024;
        D_8016D400 = fe | 2;
        return;
    }
    flags = D_8016D400;
    if ((flags & 0x20) != 0) {
        SD_SEPlayFull(45);
        SD_BGMFadeOut();
        Fade_WaitOut();
        D_8016D400 = D_8016D400 | 0x10;
        return;
    }
    if ((flags & 0x80) != 0) {
        caret = (DialogCaret *)func_80042B40(6);
        if (caret == 0) {
            return;
        }
        if ((caret->status & 0x40) == 0) {
            return;
        }
        func_8004036C(caret);
        ff = D_8016D400;
        D_8016D400 = ff & 0x7F;
        Text_SjisToGlyphCodes(D_801B125A, D_8016D418, 6);
        TextBox_Create(3, 254, 112, 204, 96, 16);
        panel = D_800EB0F8;
        panel[3].field_5A = 16;
        panel[3].field_5B = 16;
        func_80039A60((u8 *)&panel[3]);
        NameEntry_AdjustLength(1, 6);
        return;
    }
    if (func_80042B40(1) != 0) {
        return;
    }
    if (func_80042B40(2) != 0) {
        return;
    }
    if ((D_8016D400 & 0x40) == 0) {
        NameEntry_UpdateKeyboard();
        return;
    }
    term = TEXT_STRING_TERMINATOR;
    p = D_801B125A;
    c = *p;
    next = 0;
    goto ztest;
scan:
    if (*p >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
        p++;
    }
    next = p + 1;
    p = next;
    goto load;
zloop:
    p++;
load:
    c = *p;
ztest:
    if (c == 0) {
        goto zloop;
    }
    if (c != TEXT_STRING_TERMINATOR) {
        goto scan;
    }
    fg = D_8016D400;
    D_8016D400 = fg & 0xBF;
    if (next != 0) {
        *next = term;
        D_8016D4D2 = 0x80F5;
        D_8016D41C = 0;
        D_8016D400 |= 0x20;
        SD_SEPlayFull(48);
        return;
    }
    SD_SEPlayFull(9);
}

s32 NameEntry_PollCompletion(void)
{
    NameEntry_UpdateDialog();
    return D_8016D400 & 0x10;
}
