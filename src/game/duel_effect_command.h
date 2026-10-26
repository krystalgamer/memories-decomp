#ifndef MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H
#define MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H

#include "../types.h"
#include "duel_effect.h"

/* Secondary command zero: reads a modifier byte and expands the selected
 * card's name/description, type or guardian field, or applies its staged
 * color. The card id is the existing gDuel_wSelectedCardID, not a new operand.
 *
 * The stream-slot offset first lives in `text`; a block-scoped stream pointer
 * advances from the object base and is then coalesced back into `text`. That
 * lifetime split preserves the retail allocation without hard-register
 * bindings. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object);

/* Shared by the guardian and one-byte symbol wrappers, not a table entry:
 * raises the regional symbol flag (0x80, Europe 0x100), appends the glyph,
 * clears that flag and advances the pen by 0x10. */
void Text_EmitSymbolGlyph(DuelEffectChannel *object, s32 value);

/* Secondary command eight: emits the staged guardian glyph without consuming
 * a stream operand. */
void Text_EmitGuardianStarGlyph(DuelEffectChannel *object);

/* Secondary command eleven: consumes one symbol-code byte before emission. */
void Text_EmitNextSymbolGlyph(DuelEffectChannel *object);
/* Secondary command one: reset X, then add a signed operand byte to Y.
 * Both pen fields retain unsigned halfword storage and wrapping. */
void Text_ResetPenXAndAdvanceY(DuelEffectChannel *object);
/* Secondary command two: add an unsigned operand byte to X. */
void Text_AdvancePenX(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: formats a decimal number into the object's text. The value
 * comes through TextStream_ReadU32LE as a pointer, the width from the low
 * nibble of the next stream byte, and Text_EncodeDecimalDigits does the
 * conversion into a local buffer before the glyphs are appended. */
void Text_PushFormattedDecimal(DuelEffectChannel *object);

/* Secondary command four: regional font mode, not one universal size enum.
 * NA handles modes 1/2, Japan stores operand * 8 into both bytes, and Europe
 * has four size/low-flag cases. Pre-switch flag clears and unknown-mode
 * partial behavior remain distinct. */
void Text_SetFontMode(DuelEffectChannel *object);

/* Secondary command five: separate byte reads/stores for width then height;
 * no font-flag update or metric bound is implied. */
void Text_SetGlyphCellSize(DuelEffectChannel *object);
/* Secondary command 6: read the channel-local glyph pen X from two stream
 * bytes. Wrapping remains a separate operation. */
void Text_SetPenX(DuelEffectChannel *object);
#ifdef VERSION_EUROPE
/* PAL-only two-byte pen-Y operand; no opcode index is assigned here. */
extern void Text_SetPenY(DuelEffectChannel *object) asm("func_80038168");
#endif
/* Secondary command 7: reset progress and set a glyph (NA/Japan) or pixel
 * (Europe) completion limit. Zero disables this limit check. */
void Text_SetOutputLimit(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: resolves the text bank pointer for the string id in
 * gText_StagedStringId. North America selects three banks; Japan selects D_801D6000
 * for ids with bit 15 set and D_801C0000 otherwise. It hands back the slot.
 *
 * Its parameter is the DuelEffectChannel whose stream_index byte selects one of
 * the pointer words at the front of the record. It returns the selected slot,
 * although the generic command table discards that result. The slot holds
 * a stored byte-stream pointer, not an integer payload word. */
u8 *G32 *Text_PushStagedString(DuelEffectChannel *channel);

/* gText_SecondaryCommandHandlers entry: sets the effective colour index at +0x54 from a one-byte
 * operand. With bit 7 set the low nibble indexes gText_abColorSlots instead of
 * being used directly, so a palette entry and a literal colour share one
 * opcode. */
void Text_SetColorIndex(DuelEffectChannel *object);

/* Script fast-print command: clears bit 0x1000 of the object's 0x34 flags and sets it
 * again when its one-byte operand is non-zero. The selected stream uses the
 * shared pointer-slot view; the original register-qualified locals remain. */
void Text_SetFastPrintMode(DuelEffectChannel *object);

#endif
