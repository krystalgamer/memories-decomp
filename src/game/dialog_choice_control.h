#ifndef GAME_DIALOG_CHOICE_CONTROL_H
#define GAME_DIALOG_CHOICE_CONTROL_H

#include "../types.h"

#define DIALOG_CHOICE_CONTROL_MASK 0xF0
#define DIALOG_CHOICE_CONTROL_LAYOUT_MASK 0x30
#define DIALOG_CHOICE_CONTROL_RETAIN_HIGHLIGHT 0x40

/* The text engine's control byte, shared with the dialog and duel-effect
 * screens. Text_HandleChoiceCommand sets it from a command nibble
 * (`c & 0xF0`), text_stream_commands.c reads it whole, and the other two test
 * it by mask -- 0x30 for the layout arm and 0x40 for the choice arm.
 * Layout bits remain a raw masked offset in geometry. A nonzero value also
 * makes the completed text-choice layout publish a count of two.
 *
 * Script_OpSavePrompt also clears it, spelled with a .data section
 * attribute because it addresses the byte outside small data. */
extern u8 gDialog_bChoiceControl;

#endif
