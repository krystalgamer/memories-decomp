#ifndef MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_effect_states.h"

/* Reads one byte from the selected text stream and advances its cursor. */
u8 TextStream_ReadByte(DuelEffectChannel *object);

/* The builder's shared current command/glyph code. Its volatile common
 * definition stays in text_box_build_step.c; the extension handler retains
 * its local volatile pointer and publishes the combined code before -1. */
extern volatile u16 gText_CurrentStreamCode;

/* Three text stream command handlers, all taking the object that owns the
 * streams.
 *
 * Text_TryCompleteChoiceLayout reads the channel through a volatile local
 * copy of its parameter: the halfword at 0x34 is read through a volatile
 * access but written through the plain parameter so the store can fill the
 * branch delay slot. The qualifier preserves access order rather than making
 * a claim about the caller's storage.
 *
 * Text_ExtendGlyphCode and Text_SetStateFromStream both advance the
 * channel's selected stream, streams[stream_index]. */
void Text_TryCompleteChoiceLayout(DuelEffectChannel *channel);
void Text_ExtendGlyphCode(DuelEffectChannel *owner);
void Text_SetStateFromStream(DuelEffectChannel *object);

/* Writes TextStream_ReadU16LE's result into the low halfword of the same word those
 * two advance: it indexes the words at the front of the object by the signed
 * byte at 0x58, and preserves the high halfword. Declared beside them because
 * it is the third reader of that selector.
 *
 * Both callers already spelled it this way -- Text_HandleChoiceCommand
 * calls it, and duel_effect_command_table.c takes its address for the command
 * table, which is why the declaration there is not extern. */
void Text_SetCursorOffset(DuelEffectChannel *object);

/* The dialog's pending completion callback, declared here because
 * Text_TryCompleteChoiceLayout is the only function ever stored in it.
 *
 * Text_HandleChoiceCommand installs it when a choice starts waiting for input,
 * Text_TryCompleteChoiceLayout clears it once the choice resolves,
 * TextBox_BuildStep clears it when a new box starts, and Text_NewLine is the
 * only caller. Nothing else writes it, so the parameter type is not a guess:
 * it is Text_TryCompleteChoiceLayout's own. */
extern void (*gText_ChoiceLayoutCallback)(DuelEffectChannel *object);
/* Sixteen handlers for stream bytes 0xF0-0xFF, defined directly after the
 * secondary-command table in duel_effect_command_table.c. TextBox_BuildStep
 * passes each the channel it is building. */
extern void (*gText_apfnPrimaryCommands[])(DuelEffectChannel *);

#endif
