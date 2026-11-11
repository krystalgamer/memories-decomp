#include "../types.h"

#include "../psyq/libapi.h"
#ifdef VERSION_JAPAN
#include "../psyq/libetc.h"
#endif
#include "main.h"
#include "input.h"
#include "graphics_frame_controls.h"

/* Zero-initialized storage this unit owns. Each build's generated
 * input_pads_commons subsegments place it at the retail addresses. */
u32 gInput_dwPreviousHeld;
volatile u16 gInput_awPadRepeat[INPUT_PAD_COUNT];
volatile u16 gInput_awPadPressed[INPUT_PAD_COUNT];
volatile u16 gInput_awPadHeld[INPUT_PAD_COUNT];
u8 gInput_bRepeatDelay;
u8 gInput_bRepeatInterval;
u16 gInput_wPad1RepeatBackup;
u16 gInput_wPad1PressedBackup;
u16 gInput_wPad1HeldBackup;
u32 gInput_dwPendingHeld;
u32 gInput_dwDeferredRepeat;
u32 gInput_dwDeferredPressed;
#ifndef VERSION_JAPAN
InputRawPadPacket gInput_abRawPadBuffers[INPUT_PAD_COUNT];
#endif
u8 gInput_abRepeatTimers[INPUT_REPEAT_TIMER_COUNT];

/* The controller runtime: reset, regional SDK startup, and the per-frame raw-packet
   decode and held/pressed/repeat publication. All four operate on the
   gInput_* state input.h describes, and each hands that state to the next -
   Input_InitPads ends by calling Input_ResetPads, and Input_ReadRawPads fills
   the gInput_dwPendingHeld word Input_UpdatePads consumes. The pad-1/pad-2
   swap pair that follows, which reads and rewrites the six pad masks
   Input_UpdatePads publishes, is Input_BackupPad1AndUsePad2 and
   Input_RestorePad1FromBackup at the end of this unit.

   The four volatile guards are the decode/publish pair's. The two repeat
   masks' guards went with the swap pair: the remaining code builds the same
   without them. Input_ResetPads and Input_InitPads never read those masks by
   value, so the guards leave their code unchanged.

   Input_InitPads was recorded at gcc_2_8_1_g8 against gcc_2_8_1_g8_split for
   the other three. That was never a boundary: the two profiles differ only in
   -msplit-addresses, and Input_InitPads compiles to an identical object under
   either, so the unit builds at gcc_2_8_1_g8_split. It now builds at
   gcc_2_8_1_g8_split_comm, which differs from that profile only in keeping
   the storage above common for the generated subsegments to place. */

void Input_ResetPads(void) {
    s32 i = INPUT_PAD_COUNT - 1;
    volatile u16 *c = &gInput_awPadHeld[1];
    volatile u16 *b = &gInput_awPadPressed[1];
    volatile u16 *a = &gInput_awPadRepeat[1];
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
#ifdef VERSION_JAPAN
    PadInit(0);
#else
    InitPAD(
        (char *)&gInput_abRawPadBuffers[0],
        INPUT_RAW_PAD_BUFFER_SIZE,
        (char *)&gInput_abRawPadBuffers[1],
        INPUT_RAW_PAD_BUFFER_SIZE
    );
    StartPAD();
#endif
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
    Input_ResetPads();
}

void Input_ReadRawPads(void)
{
#ifdef VERSION_JAPAN
    gInput_dwPendingHeld |= PadRead(2);
#elif defined(VERSION_EUROPE)
    InputRawPadPacket *p = gInput_abRawPadBuffers;
    u32 pending = 0;

    if (p[0].status == 0)
        pending = ((p[0].buttons_hi << 8) | p[0].buttons_lo) ^
                  INPUT_PAD_BUTTON_MASK;
    if (p[1].status == 0)
        pending |=
            ((((p[1].buttons_hi << 8) |
               p[1].buttons_lo) ^ INPUT_PAD_BUTTON_MASK) <<
             INPUT_PAD_BUTTON_BITS);
    gInput_dwPendingHeld |= pending;
#else
    InputRawPadPacket *p = gInput_abRawPadBuffers;
    if (p[0].status == 0 && (p[0].format & 0xF) != 0)
        gInput_dwPendingHeld |=
            ((p[0].buttons_hi << 8) | p[0].buttons_lo) ^ INPUT_PAD_BUTTON_MASK;
    {
        InputRawPadPacket *q = gInput_abRawPadBuffers;
        if (q[1].status == 0 && (q[1].format & 0xF) != 0)
            gInput_dwPendingHeld |=
                ((((q[1].buttons_hi << 8) |
                   q[1].buttons_lo) ^ INPUT_PAD_BUTTON_MASK) <<
                 INPUT_PAD_BUTTON_BITS);
    }
#endif
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
            value = gInput_abRepeatTimers[i] + (u8)gGraphics_nBaseFrameStep;
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

    if (gGraphics_dwSyncVBlankCounter != 0) {
        gInput_dwDeferredRepeat |= repeat;
        gInput_dwDeferredPressed |= newly_pressed;
    } else {
        newly_pressed |= gInput_dwDeferredPressed;
        repeat |= gInput_dwDeferredRepeat;
        gInput_dwDeferredRepeat = 0;
        gInput_dwDeferredPressed = 0;
    }
    gInput_awPadHeld[0] = current;
    gInput_awPadHeld[1] = current >> INPUT_PAD_BUTTON_BITS;
    gInput_awPadPressed[0] = newly_pressed;
    gInput_awPadPressed[1] = newly_pressed >> INPUT_PAD_BUTTON_BITS;
    gInput_awPadRepeat[0] = repeat;
    gInput_awPadRepeat[1] = repeat >> INPUT_PAD_BUTTON_BITS;
}

void Input_BackupPad1AndUsePad2(void)
{
    u16 value;

    value = *(volatile u16 *)&gInput_awPadHeld[0];
    gInput_wPad1HeldBackup = value;
    value = *(volatile u16 *)&gInput_awPadPressed[0];
    gInput_wPad1PressedBackup = value;
    value = *(volatile u16 *)&gInput_awPadRepeat[0];
    gInput_wPad1RepeatBackup = value;
    value = gInput_awPadHeld[1];
    gInput_awPadHeld[0] = value;
    value = gInput_awPadPressed[1];
    gInput_awPadPressed[0] = value;
    value = gInput_awPadRepeat[1];
    gInput_awPadRepeat[0] = value;
}

void Input_RestorePad1FromBackup(void)
{
    u16 value;

    value = gInput_wPad1HeldBackup;
    gInput_awPadHeld[0] = value;
    value = gInput_wPad1PressedBackup;
    gInput_awPadPressed[0] = value;
    value = gInput_wPad1RepeatBackup;
    gInput_awPadRepeat[0] = value;
}
