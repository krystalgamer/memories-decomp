#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "dialog_highlight_choice.h"
#include "display_object.h"
#include "display_object_api.h"
#include "input.h"
#include "sound.h"
#include "display_object_helpers.h"
#include "../unmatched.h"
#include "dialog_choice.h"
#include "dialog_read_choice_input.h"

extern void Widget_UpdatePulseColour(u8 *);

void Dialog_UpdateChoice(u8 *p) {
    DisplayObject *e;
    s32 f;
    s32 g;
    s32 m;

    if ((p[0x51] & 0x80) == 0) {
        p[0x51] = p[0x51] | 0x80;
        e = func_800400AC((s32)func_8004006C(), 4);
        func_800427DC((u8 *)e, 1);
        func_80042918(e);
        func_800428EC((u8 *)e, (s8)(*(*(u8 **)(p + 0x28) + 0x16) + 1));
        *(DisplayObject **)(p + 0x30) = e;
        *(s32 *)&e->field_54 = 0x2000;
        e->field_4C = 0x2000;
        e->field_34 = 0x2000;
        e->field_2C.word = 0x2000;
        e->field_44.word = 0xC000;
        e->field_3C.word = 0xC000;
        e->update = Widget_UpdatePulseColour;
        e->attribute = e->attribute | (GsALON | GsAONE);
        Dialog_HighlightChoice(p);
    }

    if ((*(u16 *)(p + 0x34) & 4) != 0) {
        return;
    }

    f = *(s8 *)&gDialog_bInputState;
    g = gDialog_bInputState;

    if (f != 0) {
        if ((g & 0x40) != 0) {
            gDialog_bInputState = g & 0xBF;
            gDialog_bChoice = g & 7;
            Dialog_HighlightChoice(p);
            return;
        }
        if ((f & 0x80) == 0) {
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

    p[0x51] = 0;
    SD_SEPlayFull(7);
    if ((D_8009B34C & 0x40) == 0) {
        func_8004036C(*(DisplayObject **)(p + 0x30));
        *(DisplayObject **)(p + 0x30) = 0;
        p[0x51] = 3;
    }
}
