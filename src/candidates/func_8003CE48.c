/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * 3 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/input_pads.c.
 */
#define D_8009B0C8_IN_DATA
#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD2_HELD_IS_VOLATILE
#define GINPUT_PAD2_PRESSED_IS_VOLATILE
#define GINPUT_PAD2_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "../types.h"
#include "../psyq/libapi.h"
#include "../game/graphics_frame.h"
#include "../game/input.h"

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

extern volatile unsigned short gInput_wPad1RepeatBackup;
extern volatile unsigned short gInput_wPad1PressedBackup;
extern volatile unsigned short gInput_wPad1HeldBackup;

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
