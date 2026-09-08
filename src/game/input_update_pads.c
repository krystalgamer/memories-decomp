#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define D_8009B0D8_IN_DATA
#include "../types.h"
#include "graphics_frame.h"
#include "input.h"

/* Retail rematerializes this address inside the repeat loop. */
extern u32 D_8009B0C8[];
/* Preserve the low-half/high-half publication order at the function tail. */
extern volatile u16 gInput_wPad1Repeat;
extern volatile u16 gInput_wPad2Repeat;
extern volatile u16 gInput_wPad2Pressed;
extern volatile u16 gInput_wPad2Held;

void Input_ReadRawPads(void)
{
    u8 *p = gInput_abRawPadBuffers;
    if (p[0] == 0 && (p[1] & 0xF) != 0)
        gInput_dwPendingHeld |= ((p[2] << 8) | p[3]) ^ INPUT_PAD_BUTTON_MASK;
    {
        u8 *q = gInput_abRawPadBuffers;
        if (q[INPUT_RAW_PAD_BUFFER_SIZE] == 0 &&
            (q[INPUT_RAW_PAD_BUFFER_SIZE + 1] & 0xF) != 0)
            gInput_dwPendingHeld |=
                ((((q[INPUT_RAW_PAD_BUFFER_SIZE + 2] << 8) |
                   q[INPUT_RAW_PAD_BUFFER_SIZE + 3]) ^ INPUT_PAD_BUTTON_MASK) <<
                 INPUT_PAD_BUTTON_BITS);
    }
}

void Input_UpdatePads(void)
{
    s32 i;
    u32 repeat;
    u32 current;
    u32 newly_pressed;
    u32 held;
    u32 new_bits;
    u8 value;

    repeat = 0;
    current = gInput_dwPendingHeld;
    gInput_dwPendingHeld = 0;
    held = current;
    newly_pressed = (gInput_dwPreviousHeld ^ current) & current;
    new_bits = newly_pressed;
    gInput_dwPreviousHeld = current;

    for (i = INPUT_REPEAT_TIMER_COUNT - 1; i >= 0; i--) {
        repeat <<= 1;
        if (held & INPUT_PENDING_HIGH_BIT) {
            if (new_bits & INPUT_PENDING_HIGH_BIT) {
                repeat |= 1;
            }
            value = gInput_abRepeatTimers[i] + D_8009B0D8;
            gInput_abRepeatTimers[i] = value;
            if (value >= gInput_bRepeatDelay) {
                gInput_abRepeatTimers[i] = gInput_bRepeatInterval;
                repeat |= 1;
            }
        } else {
            gInput_abRepeatTimers[i] = 0;
        }
        held <<= 1;
        new_bits <<= 1;
    }

    if (D_8009B0C8[0] != 0) {
        gInput_dwDeferredRepeat |= repeat;
        gInput_dwDeferredPressed |= newly_pressed;
    } else {
        newly_pressed |= gInput_dwDeferredPressed;
        repeat |= gInput_dwDeferredRepeat;
        gInput_dwDeferredRepeat = 0;
        gInput_dwDeferredPressed = 0;
    }
    gInput_wPad1Held = current;
    gInput_wPad2Held = current >> INPUT_PAD_BUTTON_BITS;
    gInput_wPad1Pressed = newly_pressed;
    gInput_wPad2Pressed = newly_pressed >> INPUT_PAD_BUTTON_BITS;
    gInput_wPad1Repeat = repeat;
    gInput_wPad2Repeat = repeat >> INPUT_PAD_BUTTON_BITS;
}
