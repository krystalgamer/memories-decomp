#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "dialog_highlight_choice.h"
#include "display_object.h"
#include "display_object_core.h"
#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "input.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "dialog_choice.h"
#include "dialog_read_choice_input.h"
#include "dialog_choice_state.h"
#include "duel_effect.h"
#include "widget_update_pulse_colour.h"

void Dialog_UpdateChoice(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 f;
    s32 g;
    s32 m;

    if ((p->state_51 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        p->state_51 = p->state_51 | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        e = DisplayObject_AcquireSlot((s32)DisplayObject_FindFreeSlot(), 4);
        func_800427DC(e, 1);
        func_80042918(e);
        DisplayObject_SetDepthOffset((u8 *)e,
                      (s8)(*((u8 *)p->field_28 + 0x16) + 1));
        p->field_30 = e;
        *(s32 *)&e->field_54 = 0x2000;
        e->field_4C = 0x2000;
        e->field_34.word = 0x2000;
        e->field_2C.word = 0x2000;
        e->field_44.word = 0xC000;
        e->field_3C.word = 0xC000;
        e->update = Widget_UpdatePulseColour;
        e->attribute = e->attribute | (GsALON | GsAONE);
        Dialog_HighlightChoice(p);
    }

    if ((p->flags_34 & 4) != 0) {
        return;
    }

    f = *(s8 *)&gDialog_bInputState;
    g = gDialog_bInputState;

    if (f != 0) {
        if ((g & DIALOG_CHOICE_INPUT_CONFIRMED) != 0) {
            gDialog_bInputState = g & 0xBF;
            gDialog_bChoice = g & 7;
            Dialog_HighlightChoice(p);
            return;
        }
        if ((f & DIALOG_CHOICE_INPUT_CANCELLED) == 0) {
            return;
        }
        gDialog_bInputState = 0;
    } else {
        if (Dialog_ReadChoiceInput(p) != 0) {
            return;
        }
        if ((gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK) == 0) {
            return;
        }
    }

    m = 1 << gDialog_bChoice;
    if ((gDialog_bChoiceEnabled & m) == 0) {
        SD_SEPlayFull(9);
        return;
    }

    p->state_51 = 0;
    SD_SEPlayFull(7);
    if ((D_8009B34C & 0x40) == 0) {
        DisplayObject_ReleaseIfPresent(p->field_30);
        p->field_30 = 0;
        p->state_51 = 3;
    }
}
