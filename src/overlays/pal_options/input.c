#include "../../types.h"
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#include "../../game/input.h"
#include "../../game/sound.h"
#include "helpers.h"

void func_80168A4C(void)
{
    s32 selection = D_80169134;

    if (selection == 0) {
        if ((gInput_wPad1Pressed & PAD_DIRECTION_RIGHT) != 0 && D_80169070 == 0) {
            gSD_bOutputType = 1;
            D_80169070 = 1;
            SD_SetOutputType(1);
            SD_SEPlayFull(0x2F);
        }
        if ((gInput_wPad1Pressed & PAD_DIRECTION_LEFT) != 0 && D_80169070 == 1) {
            gSD_bOutputType = 0;
            D_80169070 = 0;
            SD_SetOutputType(0);
            SD_SEPlayFull(0x2F);
        }
    } else {
        if ((gInput_wPad1Repeat & PAD_DIRECTION_HORIZONTAL_MASK) != 0) {
            selection = D_80169140;
            if ((gInput_wPad1Repeat & PAD_DIRECTION_RIGHT) != 0) {
                selection++;
                if (selection >= 5) {
                    selection = 0;
                }
            } else {
                selection--;
                if (selection < 0) {
                    selection = 4;
                }
            }
            D_80169140 = selection;
            DisplayObject_SetResourceVariant(D_80169074, selection);
            D_80169138->field_68 = selection;
            DisplayObject_SetResourceVariant(D_80169138, D_80169138->field_69);
            D_80169078->field_68 = selection;
            DisplayObject_SetResourceVariant(D_80169078, D_80169078->field_69);
            SD_SEPlayFull(0x2F);
        }
    }
    if ((gInput_wPad1Pressed & (PAD_BUTTON_CIRCLE | PAD_BUTTON_CROSS)) != 0) {
        D_801691FC = 3;
        SD_SEPlayFull(8);
    }
}
