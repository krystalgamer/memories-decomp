#include "../types.h"
#include "input.h"

extern u8 gOptions_bState;
extern s8 gOptions_bOutputType;
extern s8 gOptions_bSelection;
/* Retail performs a fresh absolute load for each input-state test. */
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
/* Keep this byte outside small data so its store retains absolute addressing. */
extern u8 gSD_bOutputType[9];

extern void Fade_WaitOut(void);
extern void SD_SetOutputType(s32);
extern void SD_SEPlayFull(s32);
extern void Options_UpdateLayout(s32);

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
