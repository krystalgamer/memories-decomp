#ifndef MEMORIES_DECOMP_TEXT_INIT_DECIMAL_DIGIT_GLYPH_MAP_H
#define MEMORIES_DECOMP_TEXT_INIT_DECIMAL_DIGIT_GLYPH_MAP_H

#include "../types.h"
#include "../ygo_types.h"

extern TextDecimalDigitKeyBlock tent_DecimalDigitSjisKeys;
extern const u32 tent_GlyphLookupTableEntries[];

void Text_InitDecimalDigitGlyphMap(void);

#endif
