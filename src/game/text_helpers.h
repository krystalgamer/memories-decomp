#ifndef MEMORIES_DECOMP_TEXT_HELPERS_H
#define MEMORIES_DECOMP_TEXT_HELPERS_H

#include "../types.h"
#include "../ygo_types.h"
#include "text_constants.h"

extern TextDecimalDigitKeyBlock gText_DecimalDigitSjisKeys;

void Text_InitDecimalDigitGlyphMap(void);

void DuelEffect_SetEntryRangeLayout(s32 range, s32 layout);

void DuelEffect_InitEntryDefaultFlags(s32 index, s32 value);

/* Nonzero when a confirm button went down on pad 1 this frame. The value is
 * the masked button bits, not a normalized 1, so callers only test it against
 * zero. */
unsigned int Input_IsPad1ConfirmPressed(void);

#endif
