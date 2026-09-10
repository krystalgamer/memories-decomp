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

/* gInput_wPad1Pressed is declared seven different ways across the tree, and
 * six of the seven are codegen inputs rather than style. A consumer states
 * the spelling its own match needs before including this header; everything
 * else takes the plain scalar.
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
 *   _SIZED         -- eight bytes it does not have, which is how a two-byte
 *                     symbol is pushed out of small data while the one- and
 *                     two-byte scalars beside it keep %gp_rel.
 *
 * Our matching tree carries the same symbol (0x8009B398) behind the same
 * seven arms, chosen per function, which is where this list comes from. */
#ifdef GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_PRESSED_IN_DATA)
extern u16 gInput_wPad1Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_PRESSED_SIZED_VOLATILE)
extern volatile u16 gInput_wPad1Pressed[4];
#elif defined(GINPUT_PAD1_PRESSED_SIZED5)
extern u16 gInput_wPad1Pressed[5];
#elif defined(GINPUT_PAD1_PRESSED_IS_AGGREGATE)
extern u16 gInput_wPad1Pressed[];
#elif defined(GINPUT_PAD1_PRESSED_IS_VOLATILE)
extern volatile u16 gInput_wPad1Pressed;
#else
extern u16 gInput_wPad1Pressed;
#endif

/* gInput_wPad1Held is declared four different ways, and three of them are
 * codegen inputs. Same arms as gInput_wPad1Pressed above, same reasons; our
 * matching tree carries this symbol (0x8009B3A4) behind an equivalent set,
 * with a note on each recording which function needed it. */
#ifdef GINPUT_PAD1_HELD_IN_DATA_VOLATILE
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

/* gInput_wPad1Repeat is declared three ways; two are codegen inputs. Same
 * arms as the two symbols above, same reasons. It has no aggregate consumer,
 * so there is no aggregate arm -- a spelling nothing in the tree uses would
 * be a guess, not a lever. */
#ifdef GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));
#elif defined(GINPUT_PAD1_REPEAT_IS_VOLATILE)
extern volatile u16 gInput_wPad1Repeat;
#else
extern u16 gInput_wPad1Repeat;
#endif

/* The pad-2 trio. Each of these sits two bytes above its pad-1 twin --
 * Repeat at 0x8009B394/0x396, Pressed at 0x398/0x39A, Held at 0x3A4/0x3A6 --
 * so every name in this family is one element of a two-element, pad-indexed
 * pair. Input_ResetPads walks that pairing directly: it takes the address of
 * each pad-2 name and steps DOWN with --, INPUT_PAD_COUNT times.
 *
 * Only the arms some consumer needs. gInput_wPad2Pressed is the only one of
 * the three with a .data consumer, so it is the only one with that arm.
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
#ifdef GINPUT_PAD2_HELD_IS_VOLATILE
extern volatile u16 gInput_wPad2Held;
#else
extern u16 gInput_wPad2Held;
#endif

#ifdef GINPUT_PAD2_PRESSED_IN_DATA_VOLATILE
extern volatile u16 gInput_wPad2Pressed __attribute__((section(".data")));
#elif defined(GINPUT_PAD2_PRESSED_IS_VOLATILE)
extern volatile u16 gInput_wPad2Pressed;
#else
extern u16 gInput_wPad2Pressed;
#endif

#ifdef GINPUT_PAD2_REPEAT_IS_VOLATILE
extern volatile u16 gInput_wPad2Repeat;
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
