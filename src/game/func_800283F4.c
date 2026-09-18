/* D_8009B0C0 is volatile here. reorg will not look past a volatile store
 * for a delay-slot candidate, so the func_80029574 call cannot take its own
 * `li a0,3`. That goes to the entry branch instead, and the call is filled
 * forward with the slide_in constant, which otherwise sits with its first
 * use three calls further down. The opening and closing slides are separate
 * variables, which keeps slide_in local to the setup block and in $s0. */
#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#define GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#define D_8009B0C0_IN_DATA_VOLATILE
#define GDUEL_WSELECTEDCARDID_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "duel_effect_state_latch.h"
#include "graphics_frame.h"
#include "func_80029574.h"
#include "input.h"
#include "fade.h"

#include "display_object_layout.h"
#include "duel_effect.h"
#define DUEL_CARD_VIEWER_ADDRESS_ALIASES
#include "duel_card_viewer.h"
#include "duel_effect_tables.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "display_object_core.h"
#include "display_object.h"
#include "display_object_helpers.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "display_object_interpolation.h"
#include "file_transfer.h"
#include "text_box_runtime.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "card_constants.h"
#include "duel_effect_card_viewer_state.h"
#define D_8009B26C_AS_SCALAR_DATA
#include "../unmatched.h"
#include "main_mode_state.h"

void DuelEffect_UpdateCardViewerState(void)
{
    s32 slide_in;
    s32 slide_out;
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

    if (DuelEffect_MarkStateInitialized() == 0) {
        slide_in = -0x400;
        D_8009B0C0 = 1;
        func_80029574(3);
        p = &D_800EA0E8[0];
        p[3].src_y = 0x100;
        p[3].src_x = 0;
        p[3].field_2C = 0;
        p[3].field_2E = 0xFF;
        func_80029164(3, (s16)gDuel_wViewerCardID);
        obj = (DisplayObject *)func_800291E0(3, -1, -1);
        adj = gDuel_bCardViewerYOffset;
        *(s16 *)&obj->field_30.h.field_30 = -0x8C;
        obj->field_20.b.field_21 = 0x80;
        obj->field_30.h.field_32 += adj;
        obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
        DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
        obj->field_60 = slide_in;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0x14);
        D_8009B24C = obj;
        obj = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPosition((u8 *)obj, 0x148, gDuel_bCardViewerYOffset + 0xE, 0, 2, 0, 0xD, 0x107);
        obj->field_60 = slide_in;
        obj->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        DisplayObject_SelectOrderingTable1(obj);
        DisplayObject_SetDepthOffset(obj, 0x14);
        DisplayObject_SavePosition((DisplayObjectSnapshot *)obj);
        obj->field_60 = slide_in;
        D_8009B240 = obj;
        D_8009B250 = 0;
        i = 0;
        stats = gDuel_adwCardStats;
        chan = D_800EB0F8;
        for (; i < 3; i++, chan++) {
            if ((chan->flags_34 & DUEL_EFFECT_CHANNEL_FLAG_ACTIVE) == 0) {
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
                func_80039A14((struct DuelEffectChannel *)box);
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
                Widget_SlideSine((DisplayObjectPosition *)obj, 0x148, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
                Widget_SlideSine((DisplayObjectPosition *)obj, 0x94, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 + 0x55;
                obj->field_60 = flags;
                if ((s16)flags >= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x94;
                    obj->field_60 = 0;
                }
            }
            pos_box = D_8009B250;
            if (pos_box != 0) {
                TextBox_SetPos(pos_box, *(s16 *)&obj->field_30.h.field_30,
                               *(s16 *)&obj->field_30.h.field_32);
            }
        }
        obj = D_8009B24C;
        speed = obj->field_60;
        if (speed != 0) {
            if (D_8009B248 & 0x10) {
                Widget_SlideSine((DisplayObjectPosition *)obj, -0x8C, *(s16 *)&obj->field_30.h.field_32, speed);
                flags = *(u16 *)&obj->field_60 - 0x55;
                obj->field_60 = flags;
                if ((s16)flags <= 0) {
                    *(s16 *)&obj->field_30.h.field_30 = 0x400;
                    obj->field_60 = 0;
                }
            } else {
                Widget_SlideSine((DisplayObjectPosition *)obj, 2, *(s16 *)&obj->field_30.h.field_32, speed);
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
        DisplayObject_ReleaseIfPresent(bg);
        func_80029528(3);
        dead_box = D_8009B250;
        if (dead_box != 0) {
            TextBox_Destroy(dead_box);
        }
        D_8009B0C0 = 0;
        gDuel_bEffectState |= 0x40;
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
    slide_out = 0x400;
    DisplayObject_SavePosition((DisplayObjectSnapshot *)D_8009B240);
    next_obj = D_8009B24C;
    D_8009B240->field_60 = slide_out;
    DisplayObject_SavePosition((DisplayObjectSnapshot *)next_obj);
    D_8009B24C->field_60 = slide_out;
    Fade_SetTargetLevel(0xFF, 2);
    SD_SEPlayFull(0x34);
    D_8009B248 |= 0x50;
}
