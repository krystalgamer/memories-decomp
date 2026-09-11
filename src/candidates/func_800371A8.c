/* Reclassified from matching_c (#3859). This was
 * src/game/dialog_update_choice.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g1_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G1). Under gcc_2_8_1_g8, a single threshold,
 * it is 108 instructions against the target's 109, opcode distance 1. The
 * source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/dialog_highlight_choice.h"
#include "../game/display_object.h"
#include "../game/display_object_api.h"
#include "../game/input.h"
#include "../game/sound.h"
#include "../game/display_object_helpers.h"
#include "../unmatched.h"
#include "../game/dialog_choice.h"
#include "../game/dialog_read_choice_input.h"
#include "../game/dialog_choice_state.h"
#include "../game/duel_effect.h"
#include "../game/widget_update_pulse_colour.h"

void Dialog_UpdateChoice(DuelEffectChannel *p) {
    DisplayObject *e;
    s32 f;
    s32 g;
    s32 m;

    if ((p->state_51 & 0x80) == 0) {
        p->state_51 = p->state_51 | 0x80;
        e = func_800400AC((s32)func_8004006C(), 4);
        func_800427DC(e, 1);
        func_80042918(e);
        func_800428EC((u8 *)e,
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
        Dialog_HighlightChoice((u8 *)p);
    }

    if ((p->flags_34 & 4) != 0) {
        return;
    }

    f = *(s8 *)&gDialog_bInputState;
    g = gDialog_bInputState;

    if (f != 0) {
        if ((g & 0x40) != 0) {
            gDialog_bInputState = g & 0xBF;
            gDialog_bChoice = g & 7;
            Dialog_HighlightChoice((u8 *)p);
            return;
        }
        if ((f & 0x80) == 0) {
            return;
        }
        gDialog_bInputState = 0;
    } else {
        if (Dialog_ReadChoiceInput((u8 *)p) != 0) {
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
        func_8004036C(p->field_30);
        p->field_30 = 0;
        p->state_51 = 3;
    }
}
