#ifndef MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H
#define MEMORIES_DECOMP_TEXT_STREAM_COMMANDS_H

#include "../types.h"
#include "../ygo_types.h"
#include "duel_effect.h"

/* Reads one byte from the selected text stream and advances its cursor. */
u8 TextStream_ReadByte(TextStreamOwner *object);

/* Three text stream command handlers, all taking the object that owns the
 * streams.
 *
 * Text_TryCompleteChoiceLayout takes a volatile pointer. That is not
 * decoration: the body reads the halfword at 0x34 through a volatile access,
 * but writes it through an ordinary halfword lvalue so the store can fill the
 * branch delay slot. The qualifier preserves access order rather than making
 * a claim about the caller's storage.
 *
 * Text_ExtendGlyphCode and Text_SetStateFromStream both advance one of the
 * byte streams held at the front of the object, chosen by the signed byte at
 * 0x58. Text_ExtendGlyphCode takes TextStreamOwner *, the record that names
 * that array and selector; the command table casts every one of its entries
 * anyway, so the typed declaration costs no call site. */
void Text_TryCompleteChoiceLayout(volatile DuelEffectChannel *object);
void Text_ExtendGlyphCode(TextStreamOwner *owner);
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
 * it is Text_TryCompleteChoiceLayout's own, volatile qualifier included. */
extern void (*D_8009B340)(volatile DuelEffectChannel *object);
extern void (*D_80090F18[])(u8 *);

#endif
