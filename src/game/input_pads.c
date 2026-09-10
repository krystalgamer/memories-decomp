#define D_8009B0C8_IN_DATA
#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD2_HELD_IS_VOLATILE
#define GINPUT_PAD2_PRESSED_IS_VOLATILE
#define GINPUT_PAD2_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#define D_8009B0D8_IN_DATA
#include "../types.h"
#include "../psyq/libapi.h"
#include "graphics_frame.h"
#include "input.h"

/* The controller runtime: reset, LIBAPI startup, the per-frame raw-packet
   decode and held/pressed/repeat publication, and the pad-1/pad-2 swap
   around it. All six operate on the gInput_* state input.h describes, and
   each hands that state to the next - Input_InitPads ends by calling
   Input_ResetPads, Input_ReadRawPads fills the gInput_dwPendingHeld word
   Input_UpdatePads consumes, and the swap pair reads and rewrites the six
   pad masks Input_UpdatePads publishes.

   The six volatile guards were already shared by the decode/publish pair
   and the swap pair. Input_ResetPads and Input_InitPads never read those
   masks by value, so the guards leave their code unchanged.

   Input_InitPads and the swap pair were recorded at gcc_2_8_1_g8 against
   gcc_2_8_1_g8_split for the other three. That was never a boundary: the
   two profiles differ only in -msplit-addresses, and each of the three
   compiles to an identical object under either, so the unit builds at
   gcc_2_8_1_g8_split. */

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

void Input_InitPads(void)
{
    InitPAD(
        gInput_abRawPadBuffers,
        INPUT_RAW_PAD_BUFFER_SIZE,
        gInput_abRawPadBuffers + INPUT_RAW_PAD_BUFFER_SIZE,
        INPUT_RAW_PAD_BUFFER_SIZE
    );
    StartPAD();
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
    Input_ResetPads();
}

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

/* Preserve the low-half/high-half publication order at the function tail. */
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

    if (D_8009B0C8 != 0) {
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

extern volatile unsigned short gInput_wPad1RepeatBackup;
extern volatile unsigned short gInput_wPad1PressedBackup;
extern volatile unsigned short gInput_wPad1HeldBackup;

void Input_BackupPad1AndUsePad2(void)
{
    unsigned short value;

    value = gInput_wPad1Held;
    __asm__ volatile("nop");
    gInput_wPad1HeldBackup = value;
    value = gInput_wPad1Pressed;
    __asm__ volatile("nop");
    gInput_wPad1PressedBackup = value;
    value = gInput_wPad1Repeat;
    __asm__ volatile("nop");
    gInput_wPad1RepeatBackup = value;
    value = gInput_wPad2Held;
    __asm__ volatile("nop");
    gInput_wPad1Held = value;
    value = gInput_wPad2Pressed;
    __asm__ volatile("nop");
    gInput_wPad1Pressed = value;
    value = gInput_wPad2Repeat;
    __asm__ volatile("nop");
    gInput_wPad1Repeat = value;
}

void Input_RestorePad1FromBackup(void)
{
    unsigned short value;

    value = gInput_wPad1HeldBackup;
    __asm__ volatile("nop");
    gInput_wPad1Held = value;
    value = gInput_wPad1PressedBackup;
    __asm__ volatile("nop");
    gInput_wPad1Pressed = value;
    value = gInput_wPad1RepeatBackup;
    __asm__ volatile("nop");
    gInput_wPad1Repeat = value;
}
