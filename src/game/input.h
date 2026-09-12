#ifndef MEMORIES_DECOMP_INPUT_H
#define MEMORIES_DECOMP_INPUT_H

#include "../types.h"

#define INPUT_PAD_COUNT 2
#define INPUT_RAW_PAD_BUFFER_SIZE 0x22
#define INPUT_PAD_BUTTON_BITS 16
#define INPUT_PAD_BUTTON_MASK ((1 << INPUT_PAD_BUTTON_BITS) - 1)
#define INPUT_REPEAT_TIMER_COUNT (INPUT_PAD_COUNT * INPUT_PAD_BUTTON_BITS)
#define INPUT_PENDING_HIGH_BIT (1U << (INPUT_REPEAT_TIMER_COUNT - 1))

#define PAD_DIRECTION_UP 0x1000
#define PAD_DIRECTION_RIGHT 0x2000
#define PAD_DIRECTION_DOWN 0x4000
#define PAD_DIRECTION_LEFT 0x8000
#define PAD_DIRECTION_MASK 0xF000
#define PAD_DIRECTION_VERTICAL_MASK \
    (PAD_DIRECTION_UP | PAD_DIRECTION_DOWN)
#define PAD_DIRECTION_HORIZONTAL_MASK \
    (PAD_DIRECTION_RIGHT | PAD_DIRECTION_LEFT)

#define PAD_BUTTON_L2 0x1
#define PAD_BUTTON_R2 0x2
#define PAD_BUTTON_TRIGGER_MASK (PAD_BUTTON_L2 | PAD_BUTTON_R2)
#define PAD_BUTTON_L1 0x4
#define PAD_BUTTON_R1 0x8
#define PAD_BUTTON_L1_R1_MASK (PAD_BUTTON_L1 | PAD_BUTTON_R1)
#define PAD_BUTTON_TRIANGLE 0x10
#define PAD_BUTTON_CIRCLE 0x20
#define PAD_BUTTON_CROSS 0x40
#define PAD_BUTTON_SQUARE 0x80
#define PAD_BUTTON_SELECT 0x100
#define PAD_BUTTON_START 0x800

#define PAD_BUTTON_CANCEL PAD_BUTTON_CIRCLE
#define PAD_BUTTON_CONFIRM_MASK (PAD_BUTTON_CROSS | PAD_BUTTON_SQUARE)

#define INPUT_REPEAT_THRESHOLD 0x18
#define INPUT_REPEAT_RELOAD_VALUE 0x14

extern u8 gInput_abRawPadBuffers[INPUT_PAD_COUNT * INPUT_RAW_PAD_BUFFER_SIZE];
extern u8 gInput_abRepeatTimers[INPUT_REPEAT_TIMER_COUNT];
extern u8 gInput_bRepeatDelay;
extern u8 gInput_bRepeatInterval;
extern u32 gInput_dwPreviousHeld;
extern u32 gInput_dwDeferredRepeat;
extern u32 gInput_dwDeferredPressed;
extern u32 gInput_dwPendingHeld;

/* gInput_wPad1Pressed is declared six different ways below, counting the
 * plain scalar; the other five are codegen inputs rather than style. A
 * consumer states the spelling its own match needs before including this
 * header; everything else takes the plain scalar.
 *
 * Two independent knobs, so arms rather than one chain: a file that wants the
 * non-small form *and* the re-reads would silently lose the volatile if these
 * shared a single ladder.
 *
 *   _IS_VOLATILE   -- the halfword is re-read on each path. Without it gcc
 *                     commons several reads into one register.
 *   _IN_DATA       -- takes the symbol out of small data at the compiler,
 *                     with its true size: the bare form the assembler
 *                     expands, rather than cc1psx's own %hi/%lo pair.
 *   _IS_AGGREGATE  -- an unsized array is not small data either, but gives
 *                     cc1psx's split pair instead of the bare symbol.
 *   _SIZED_VOLATILE -- eight bytes it does not have, and volatile. That
 *                     leaves small data only where the assembler's -G sits
 *                     below the compiler's. Its consumers are
 *                     src/candidates/func_800307B8.c, which records that it
 *                     was byte-exact under gcc_2_8_1_cc_g8_as_g4_split and
 *                     is 117 instructions against the target's 120 at
 *                     gcc_2_8_1_g8, and src/candidates/func_80030294.c,
 *                     which is under the same profile and reads all six
 *                     pad names this way.
 *
 * A `[5]` arm used to sit beside that one, for src/candidates/func_80017034.c
 * under gcc_2_8_1_g8_split. `[5]` and an unknown size are both outside small
 * data at a single -G8 threshold, and the split flag decides only whose
 * %hi/%lo pair the reference becomes, so nothing separated the two
 * spellings: moving that unit to the aggregate arm leaves the retail SHA-256
 * unchanged and check-candidate-builds green.
 *
 * An independent matching decompilation of this binary carries the same
 * address behind eight declarations, chosen per function, which is where this
 * list came from. Its sized arms are `[4]`, volatile and not, and it has no
 * `[5]`. */
#ifdef GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_PRESSED_IN_DATA)
extern u16 gInput_wPad1Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_PRESSED_SIZED_VOLATILE)
extern volatile u16 gInput_wPad1Pressed[4];
#elif defined(GINPUT_PAD1_PRESSED_IS_AGGREGATE)
extern u16 gInput_wPad1Pressed[];
#elif defined(GINPUT_PAD1_PRESSED_IS_VOLATILE)
extern volatile u16 gInput_wPad1Pressed;
#else
extern u16 gInput_wPad1Pressed;
#endif

/* gInput_wPad1Held: same arms as gInput_wPad1Pressed above, same reasons;
 * our matching tree carries this symbol (0x8009B3A4) behind an equivalent
 * set, with a note on each recording which function needed it. The
 * _SIZED_VOLATILE arm is src/candidates/func_80030294.c's, which reads this
 * name and the other five at [0] under gcc_2_8_1_cc_g8_as_g4_split; that
 * unit's own header records the mechanism. */
#ifdef GINPUT_PAD1_HELD_SIZED_VOLATILE
extern volatile u16 gInput_wPad1Held[4];
#elif defined(GINPUT_PAD1_HELD_IN_DATA_VOLATILE)
extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_HELD_IN_DATA)
extern u16 gInput_wPad1Held __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_HELD_IS_AGGREGATE)
extern u16 gInput_wPad1Held[];
#elif defined(GINPUT_PAD1_HELD_IS_VOLATILE)
extern volatile u16 gInput_wPad1Held;
#else
extern u16 gInput_wPad1Held;
#endif

/* gInput_wPad1Repeat: same arms as the two symbols above, same reasons, and
 * the _SIZED_VOLATILE one is func_80030294.c's. It still has no aggregate
 * consumer, so there is no unsized arm -- a spelling nothing in the tree
 * uses would be a guess, not a lever. */
#ifdef GINPUT_PAD1_REPEAT_SIZED_VOLATILE
extern volatile u16 gInput_wPad1Repeat[4];
#elif defined(GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE)
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_REPEAT_IS_VOLATILE)
extern volatile u16 gInput_wPad1Repeat;
#else
extern u16 gInput_wPad1Repeat;
#endif

/* Saved pad-1 masks used by the temporary pad-2 swap. Both the backup and the
 * restore path are in input_pad1_backup.c, which owns the COMMON definitions
 * and needs no volatile arm: the unit builds byte-identically without one. */
extern u16 gInput_wPad1HeldBackup;
extern u16 gInput_wPad1PressedBackup;
extern u16 gInput_wPad1RepeatBackup;

/* The pad-2 trio. Each of these sits two bytes above its pad-1 twin --
 * Repeat at 0x8009B394/0x396, Pressed at 0x398/0x39A, Held at 0x3A4/0x3A6 --
 * so every name in this family is one element of a two-element, pad-indexed
 * pair. Input_ResetPads walks that pairing directly: it takes the address of
 * each pad-2 name and steps DOWN with --, INPUT_PAD_COUNT times.
 *
 * Only the arms some consumer needs. gInput_wPad2Pressed is the only one of
 * the three with a .data consumer, so it is the only one with that arm; all
 * three have a _SIZED_VOLATILE arm, and all three of those are
 * src/candidates/func_80030294.c's.
 *
 * Some consumers reach pad 2 as element 1 of the pad-1 name rather than by
 * these names, and that cannot be converted. value_setup.c and
 * MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c) declare
 * `volatile u16 D_8009B394[]` and read both `[0]` and
 * `[1]`; rewriting `[1]` to gInput_wPad2Repeat/gInput_wPad2Pressed is the
 * obvious tidy-up and it does not build. Measured on the value-setup updater: the
 * main_menu module stops matching, and it still fails when only one of the two
 * symbols is converted, so it is the pad-2 access itself and not an
 * interaction between them.
 *
 * The reason is addressing, not naming. `X[1]` is one materialization of the
 * pad-1 symbol plus a displacement; the pad-2 name is its own relocation.
 * Retail chose per site, so both spellings are faithful and neither can be
 * made to stand in for the other. Same shape as the overlaps recorded in
 * notes/memory-map.md and the main_menu README. */
#ifdef GINPUT_PAD2_HELD_SIZED_VOLATILE
extern volatile u16 gInput_wPad2Held[4];
#elif defined(GINPUT_PAD2_HELD_IS_VOLATILE)
extern volatile u16 gInput_wPad2Held;
#else
extern u16 gInput_wPad2Held;
#endif

#ifdef GINPUT_PAD2_PRESSED_SIZED_VOLATILE
extern volatile u16 gInput_wPad2Pressed[4];
#elif defined(GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE)
extern volatile u16 gInput_wPad2Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD2_PRESSED_IS_VOLATILE)
extern volatile u16 gInput_wPad2Pressed;
#else
extern u16 gInput_wPad2Pressed;
#endif

#ifdef GINPUT_PAD2_REPEAT_SIZED_VOLATILE
extern volatile u16 gInput_wPad2Repeat[4];
#else
extern u16 gInput_wPad2Repeat;
#endif

void Input_ResetPads(void);
void Input_InitPads(void);
void Input_ReadRawPads(void);
void Input_UpdatePads(void);
void Input_BackupPad1AndUsePad2(void);
void Input_RestorePad1FromBackup(void);

#endif
