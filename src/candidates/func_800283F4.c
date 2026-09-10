/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * pinning 2 variables to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/func_800283F4.c.
 */
#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define D_8009B0C0_IN_DATA
#define GDUEL_WSELECTEDCARDID_IN_DATA
#include "../types.h"
#include "../game/func_800282E8.h"
#include "../game/graphics_frame.h"
#include "../game/func_80029574.h"
#include "../game/input.h"
#include "../game/fade.h"

#include "../game/display_object_layout.h"
#include "../game/duel_effect.h"
#include "../game/duel_card_viewer.h"
#include "../game/duel_effect_tables.h"
#include "../game/text_box_lifecycle.h"
#include "../game/sound.h"
#include "../game/display_object_api.h"
#include "../game/display_object.h"
#include "../game/display_object_helpers.h"
#include "../game/func_800291E0.h"
#include "../game/duel_effect_resource_setup.h"
#include "../game/func_80043178.h"
#include "../game/display_object_interpolation.h"
#include "../game/file_transfer.h"
#include "../game/text_box_runtime.h"
#include "../game/duel_card.h"
#include "../game/duel_effect_resource_record.h"
#include "../game/card_constants.h"
#include "../game/func_800283F4.h"
#include "../unmatched.h"

extern DisplayObject *D_8009B240;
extern u8 D_8009B248;
extern DisplayObject *D_8009B24C;
extern DuelEffectChannel *D_8009B250;
extern u8 D_8009B26C __attribute__((section(".data")));

void func_800283F4(void)
{
    /* Pinned: unpinned, gcc puts the slide value in $s1 and the object in
       $s0, the opposite of retail, which costs 56 positions. */
    register s32 slide __asm__("$16");
    register s32 channel __asm__("$4");
    DisplayObject *obj;
    DisplayObject *bg;
    DuelEffectResourceRecord *p;
    DisplayObject *next_obj;
    s32 *stats;
    DuelEffectChannel *box;
    DuelEffectChannel *chan;
    DuelEffectChannel *pos_box;
    DuelEffectChannel *dead_box;
    s32 adj;
    s32 i;
    s32 kind;
    s32 masked;
    s32 speed;
    s32 test;
    u16 flags;
    u16 id;
    u16 objflags;
    u8 closing;
    u8 state;

    if (func_800282E8() == 0) {
        channel = 3;
        slide = -0x400;
        flags = channel;
        D_8009B0C0 = flags - 2;
        func_80029574(flags);
        p = &D_800EA0E8[0];
        p[3].src_y = 0x100;
        p[3].src_x = 0;
        p[3].field_2C = 0;
        p[3].field_2E = 0xFF;
        func_80029164(3, (s16)gDuel_wViewerCardID);
        obj = (DisplayObject *)func_800291E0(3, -1, -1);
        adj = D_8009B24B;
        *(s16 *)&obj->field_30.h.field_30 = -0x8C;
        obj->field_20.b.field_21 = 0x80;
        obj->field_30.h.field_32 += adj;
        obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        func_80043178((DisplayObjectSnapshot *)obj);
        obj->field_60 = slide;
        func_80042918(obj);
        func_800428EC((u8 *)obj, 0x14);
        D_8009B24C = obj;
        obj = func_800400AC(func_8004002C(), 2);
        func_800404CC((u8 *)obj, 0x148, D_8009B24B + 0xE, 0, 2, 0, 0xD, 0x107);
        obj->field_60 = slide;
        obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        func_80042918(obj);
        func_800428EC((u8 *)obj, 0x14);
        func_80043178((DisplayObjectSnapshot *)obj);
        obj->field_60 = slide;
        D_8009B240 = obj;
        D_8009B250 = 0;
        i = 0;
        stats = gDuel_adwCardStats;
        chan = D_800EB0F8;
        for (; i < 3; i++, chan++) {
            if ((chan->flags_34 & 0x8000) == 0) {
                id = gDuel_wViewerCardID;
                gDuel_wSelectedCardID = id;
                kind = 3;
                if (((stats[(s16)id - 1] >> CARD_STAT_TYPE_SHIFT) &
                     CARD_STAT_TYPE_MASK) >= CARD_TYPE_MAGIC) {
                    kind = 4;
                }
                box = TextBox_Create(i, kind, 0x148, 0xE, 0xA8, 0xC0);
                box->field_53 = 1;
                box->field_54 = 0;
                box->field_59 = 0x15;
                D_8009B250 = box;
                func_80039A14((u8 *)box);
                break;
            }
        }
        Fade_SetTargetLevel(0x40, 2);
        D_8009B140 = *(u8 *)&D_8009AF74[1] - 0x13;
        SD_SEPlayFull(0x34);
        D_8009B248 |= 0x40;
        return;
    }

    state = D_8009B248;
    if (state & 0x40) {
        obj = D_8009B240;
        speed = obj->field_60;
        if (speed != 0) {
            if (state & 0x10) {
                func_80043230((DisplayObjectPosition *)obj, 0x148, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
                func_80043230((DisplayObjectPosition *)obj, 0x94, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 + 0x55;
                obj->field_60 = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x94;
                    obj->field_60 = 0;
                }
            }
            pos_box = D_8009B250;
            if (pos_box != 0) {
                TextBox_SetPos((u8 *)pos_box, *(s16 *)&obj->field_30.h.field_30,
                               *(s16 *)&obj->field_30.h.field_32);
            }
        }
        obj = D_8009B24C;
        speed = obj->field_60;
        if (speed != 0) {
            if (D_8009B248 & 0x10) {
                func_80043230((DisplayObjectPosition *)obj, -0x8C, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
                func_80043230((DisplayObjectPosition *)obj, 2, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 + 0x55;
                obj->field_60 = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 2;
                    obj->field_60 = 0;
                }
            }
        }
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            return;
        }
        bg = D_8009B240;
        if (bg->field_60 != 0) {
            return;
        }
        if (D_8009B24C->field_60 != 0) {
            return;
        }
        closing = D_8009B248;
        masked = closing & 0xBF;
        test = closing & 0x10;
        D_8009B248 = masked;
        if (test == 0) {
            return;
        }
        func_8004036C(bg);
        func_80029528(3);
        dead_box = D_8009B250;
        if (dead_box != 0) {
            TextBox_Destroy(dead_box);
        }
        D_8009B0C0 = 0;
        D_8009B254 |= 0x40;
        return;
    }

    if ((state & 0x20) == 0) {
        obj = D_8009B24C;
        obj->field_20.b.field_21 += 0xC;
        if (obj->field_20.b.field_21 < 0x40) {
            objflags = obj->flags;
            obj->field_20.b.field_21 = 0;
            state = D_8009B248 | 0x20;
            obj->flags = objflags & 0xFFFB;
            D_8009B248 = state;
        }
        return;
    }

    if ((D_8009B26C & 0x1F) == 0xE) {
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0x20) != 0) {
            goto press;
        }
        return;
    }
    if ((gInput_wPad1Pressed & 0x20) == 0) {
        return;
    }
press:
    slide = 0x400;
    func_80043178((DisplayObjectSnapshot *)D_8009B240);
    next_obj = D_8009B24C;
    D_8009B240->field_60 = slide;
    func_80043178((DisplayObjectSnapshot *)next_obj);
    D_8009B24C->field_60 = slide;
    Fade_SetTargetLevel(0xFF, 2);
    SD_SEPlayFull(0x34);
    D_8009B248 |= 0x50;
}

