#include "../types.h"
#include "input.h"

extern u16 gInput_wPad2Held, gInput_wPad2Pressed, gInput_wPad2Repeat;

void Input_ResetPads(void) {
    s32 i = INPUT_PAD_COUNT - 1;
    volatile u16 *c = &gInput_wPad2Held;
    volatile u16 *b = &gInput_wPad2Pressed;
    volatile u16 *a = &gInput_wPad2Repeat;
    u8 *base;
    do {
        *a = 0;
        *b = *a;
        *c = *b;
        a--; b--; c--; i--;
    } while (i >= 0);
    i = INPUT_REPEAT_TIMER_COUNT - 1;
    base = gInput_abRepeatTimers;
    for (; i >= 0; i--) {
        *(u8 *)((u32)i + (u32)base) = 0;
    }
    gInput_dwPreviousHeld = 0;
    gInput_dwDeferredRepeat = 0;
    gInput_dwDeferredPressed = 0;
    gInput_dwPendingHeld = 0;
}
