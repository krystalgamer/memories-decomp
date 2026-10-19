#ifndef MEMORIES_DECOMP_TEXT_INIT_DECIMAL_DIGIT_GLYPH_MAP_H
#define MEMORIES_DECOMP_TEXT_INIT_DECIMAL_DIGIT_GLYPH_MAP_H

#include "../types.h"
#include "../ygo_types.h"
#include "text_constants.h"

extern TextDecimalDigitKeyBlock tent_DecimalDigitSjisKeys;
extern const TextGlyphLookupEntry tent_GlyphLookupTableEntries[];

void Text_InitDecimalDigitGlyphMap(void);

#endif
