#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "fade.h"
#include "input.h"
#include "options_update_layout.h"
#include "options.h"
#include "sound.h"

/* Retail performs a fresh absolute load for each input-state test. */
/* Keep this byte outside small data so its store retains absolute addressing. */
extern u8 gSD_bOutputType[9];

void Options_HandleInput(void)
{
    s32 value;

    if (gOptions_bSelection == 0 &&
        (gInput_wPad1Pressed & PAD_DIRECTION_HORIZONTAL_MASK)) {
        if (gInput_wPad1Pressed & PAD_DIRECTION_RIGHT) {
            if (gOptions_bOutputType != 0) {
                return;
            }
            value = 1;
            gSD_bOutputType[0] = value;
            gOptions_bOutputType = 1;
        } else {
            if (gOptions_bOutputType == 0) {
                return;
            }
            value = 0;
            gSD_bOutputType[0] = value;
            gOptions_bOutputType = 0;
        }
        SD_SetOutputType(value);
        SD_SEPlayFull(0x2F);
        Options_UpdateLayout(gOptions_bSelection);
        return;
    }

    if (gOptions_bSelection != 0 &&
        (gInput_wPad1Pressed & PAD_BUTTON_CONFIRM_MASK)) {
        SD_SEPlayFull(7);
        gOptions_bState = *(u8 *)&gOptions_bSelection + 1;
        return;
    }

    if (gInput_wPad1Pressed & PAD_BUTTON_CANCEL) {
        gOptions_bState = 0;
        SD_SEPlayFull(8);
    }
}

s32 Options_Update(void)
{
    switch (gOptions_bState & 0xF) {
    case 0: Fade_WaitOut(); break;
    case 1: Options_HandleInput(); break;
    case 2: break;
    case 3: gOptions_bState = 1; break;
    }
    return gOptions_bState;
}
