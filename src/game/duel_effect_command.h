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
void func_800380D4(DuelEffectChannel *object);
void func_80038110(DuelEffectChannel *object);

/* tent_SecondaryTextCommandTable entry: formats a decimal number into the object's text. The value
 * comes through TextStream_ReadU32LE as a pointer, the width from the low
 * nibble of the next stream byte, and Text_EncodeDecimalDigits does the
 * conversion into a local buffer before the glyphs are appended. */
void func_80038148(DuelEffectChannel *object);

/* tent_SecondaryTextCommandTable entry: sets the object's glyph cell size from a one-byte operand.
 * North America uses 8 by 8 for 1, 8 by 12 for 2, unchanged otherwise;
 * Japan stores operand * 8 into both size bytes. Both mirror case 1 into bit
 * 0x100 of the object's 0x34 flags after clearing it. The stream read is
 * inlined so the payload and channel retain independent local lifetimes. */
void func_800382A8(DuelEffectChannel *object);

void func_80038334(DuelEffectChannel *object);
/* Secondary command 6: read the channel-local glyph pen X from two stream
 * bytes. Wrapping remains a separate operation. */
void Text_SetPenX(DuelEffectChannel *object);
/* Secondary command 7: reset progress and set a glyph (NA/Japan) or pixel
 * (Europe) completion limit. Zero disables this limit check. */
void Text_SetOutputLimit(DuelEffectChannel *object);

/* tent_SecondaryTextCommandTable entry: resolves the text bank pointer for the string id in
 * D_8009B32E. North America selects three banks; Japan selects D_801D6000
 * for ids with bit 15 set and D_801C0000 otherwise. It hands back the slot.
 *
 * Its parameter is the DuelEffectChannel whose stream_58 byte selects one of
 * the pointer words at the front of the record. It returns the selected slot,
 * although the generic command table discards that result. The slot holds
 * a stored byte-stream pointer, not an integer payload word. */
u8 *G32 *func_800383DC(DuelEffectChannel *channel);

/* tent_SecondaryTextCommandTable entry: sets the effective colour index at +0x54 from a one-byte
 * operand. With bit 7 set the low nibble indexes gText_abColorSlots instead of
 * being used directly, so a palette entry and a literal colour share one
 * opcode. */
void Text_SetColorIndex(DuelEffectChannel *object);

/* Script fast-print command: clears bit 0x1000 of the object's 0x34 flags and sets it
 * again when its one-byte operand is non-zero. The selected stream uses the
 * shared pointer-slot view; the original register-qualified locals remain. */
void Text_SetFastPrintMode(DuelEffectChannel *object);

#endif
