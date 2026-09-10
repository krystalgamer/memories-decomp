#ifndef MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H
#define MEMORIES_DECOMP_DUEL_EFFECT_COMMAND_H

#include "../types.h"
#include "duel_effect.h"

/* D_80090EAC entry: the card-name and stat text command. It reads an opcode
 * byte, resolves a card id -- gDuel_wSelectedCardID or one taken from the
 * stream -- and appends the requested field to the object's text.
 *
 * The body is register-pinned on the current stream pointer; the note in the
 * source explains which pair local-alloc otherwise swaps. */
void func_80037DA4(u8 *object);

/* The shared step every handler in this unit runs: it raises bit 0x80 of the
 * halfword at 0x34, calls func_80036C14 with the value it was given, lowers
 * the bit again and advances the halfword at 0x38 by 0x10. It is not in the
 * command table itself -- the four handlers below are its only callers. */
void func_80038024(DuelEffectChannel *object, s32 value);

/* Forwards its argument straight to func_80038024 and never indexes
 * it, which is why the definition took void * before this record was
 * named. The command table stores it in an array of u8 * handlers and
 * casts it there, as it now does for the other three. */
void func_80038070(DuelEffectChannel *object);

void func_80038094(DuelEffectChannel *object);
void func_800380D4(DuelEffectChannel *object);
void func_80038110(DuelEffectChannel *object);

/* D_80090EAC entry: formats a decimal number into the object's text. The value
 * comes through func_80036D70 as a pointer, the width from the low nibble of the
 * next stream byte, and Text_EncodeDecimalDigits does the conversion into a
 * local buffer before the glyphs are appended. */
void func_80038148(u8 *object);

/* D_80090EAC entry: sets the object's glyph cell size from a one-byte operand --
 * 8 by 8 for 1, 8 by 12 for 2, unchanged otherwise -- and mirrors case 1 into
 * bit 0x100 of the object's 0x34 flags, which it clears first either way. The
 * body is register-pinned throughout. */
void func_800382A8(u8 *object);

void func_80038334(DuelEffectChannel *object);
void func_80038388(DuelEffectChannel *object);
void func_800383B0(DuelEffectChannel *object);

/* D_80090EAC entry: resolves the text bank pointer for the string id in
 * D_8009B32E -- above 0xCFFF from D_801C0000, above the global-string base from
 * D_801D5800, otherwise the campaign bank -- and hands back the slot it found.
 *
 * Its parameter is the DuelEffectChannel whose stream_58 byte selects one of
 * the pointer words at the front of the record. It returns the selected slot,
 * although the generic command table discards that result. */
u32 *func_800383DC(DuelEffectChannel *channel);

/* D_80090EAC entry: sets the object's colour slot at +0x54 from a one-byte
 * operand. With bit 7 set the low nibble indexes gText_abColorSlots instead of
 * being used directly, so a palette entry and a literal colour share one
 * opcode. */
void func_80038498(u8 *object);

/* D_80090EAC entry: clears bit 0x1000 of the object's 0x34 flags and sets it
 * again when its one-byte operand is non-zero. Register-pinned. */
void func_800384E4(u8 *object);

#endif
