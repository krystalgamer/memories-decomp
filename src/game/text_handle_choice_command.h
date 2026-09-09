#ifndef MEMORIES_DECOMP_TEXT_HANDLE_CHOICE_COMMAND_H
#define MEMORIES_DECOMP_TEXT_HANDLE_CHOICE_COMMAND_H

#include "../types.h"

/* D_80090EAC entry: sets up a dialog choice. It reads a control byte, and a
 * second one when bit 3 is set, then either advances the object's stream cursor
 * by the already-made choice and re-runs Text_SetCursorOffset (bit 7), or
 * publishes a new choice -- count from the low three bits, style from the high
 * nibble, enabled mask from the second byte -- and puts the dialog input state
 * at 1 when that byte's bit 7 is set. Always raises D_8009B350. */
void Text_HandleChoiceCommand(u8 *object);

#endif
