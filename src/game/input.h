#ifndef MEMORIES_DECOMP_INPUT_H
#define MEMORIES_DECOMP_INPUT_H

#include "../types.h"

#define INPUT_PAD_COUNT 2
/* 0x24 in the European build, whose wrapper defines it. */
#ifndef INPUT_RAW_PAD_BUFFER_SIZE
#define INPUT_RAW_PAD_BUFFER_SIZE 0x22
#endif
#define INPUT_PAD_BUTTON_BITS 16
#define INPUT_PAD_BUTTON_MASK ((1 << INPUT_PAD_BUTTON_BITS) - 1)
#define INPUT_REPEAT_TIMER_COUNT (INPUT_PAD_COUNT * INPUT_PAD_BUTTON_BITS)
#define INPUT_PENDING_HIGH_BIT (1U << (INPUT_REPEAT_TIMER_COUNT - 1))

/* Only the four bytes consumed by Input_ReadRawPads are interpreted. */
typedef struct InputRawPadPacket {
    u8 status;
    u8 format;
    u8 buttons_hi;
    u8 buttons_lo;
    u8 payload[INPUT_RAW_PAD_BUFFER_SIZE - 4];
} InputRawPadPacket;

typedef char InputRawPadPacket_size_must_match_receive_buffer[
    sizeof(InputRawPadPacket) == INPUT_RAW_PAD_BUFFER_SIZE ? 1 : -1
];

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

/* The European build's wrappers define their own (0x14 and 0x11). */
#ifndef INPUT_REPEAT_THRESHOLD
#define INPUT_REPEAT_THRESHOLD 0x18
#endif
#ifndef INPUT_REPEAT_RELOAD_VALUE
#define INPUT_REPEAT_RELOAD_VALUE 0x14
#endif

extern InputRawPadPacket gInput_abRawPadBuffers[INPUT_PAD_COUNT];
extern u8 gInput_abRepeatTimers[INPUT_REPEAT_TIMER_COUNT];
extern u8 gInput_bRepeatDelay;
extern u8 gInput_bRepeatInterval;
/* The Spanish Dialog_ReadChoiceInput reaches it through $at. */
#ifdef GINPUT_PREVIOUS_HELD_IN_DATA
extern u32 gInput_dwPreviousHeld __attribute__((section(".data")));
#else
extern u32 gInput_dwPreviousHeld;
#endif
extern u32 gInput_dwDeferredRepeat;
extern u32 gInput_dwDeferredPressed;
extern u32 gInput_dwPendingHeld;

/* Per-pad button masks, indexed by pad (0 = pad 1, 1 = pad 2), published by
 * Input_UpdatePads each frame: held buttons, newly pressed buttons, and the
 * repeat stream (new presses plus auto-repeat). Each is one two-element
 * array -- Repeat at 0x8009B394, Pressed at 0x8009B398, Held at 0x8009B3A4 --
 * and Input_ResetPads walks them that way, taking &array[1] and stepping
 * down. They are volatile: the readers re-load them on every test. */
extern volatile u16 gInput_awPadPressed[INPUT_PAD_COUNT];
extern volatile u16 gInput_awPadHeld[INPUT_PAD_COUNT];
extern volatile u16 gInput_awPadRepeat[INPUT_PAD_COUNT];

/* Saved pad-1 masks used by the temporary pad-2 swap. Both the backup and the
 * restore path are in input_pads.c, which defines them in its small data. */
extern u16 gInput_wPad1HeldBackup;
extern u16 gInput_wPad1PressedBackup;
extern u16 gInput_wPad1RepeatBackup;

void Input_ResetPads(void);
void Input_InitPads(void);
void Input_ReadRawPads(void);
void Input_UpdatePads(void);
void Input_BackupPad1AndUsePad2(void);
void Input_RestorePad1FromBackup(void);

#endif
