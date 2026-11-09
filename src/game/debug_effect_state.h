#ifndef MEMORIES_DECOMP_DEBUG_EFFECT_STATE_H
#define MEMORIES_DECOMP_DEBUG_EFFECT_STATE_H

#include "../types.h"

/* The developer effect-preview screen's state: the index of the operand being
 * edited, the two tuned operands and the page. The operand pair is a byte
 * array, which GCC word-aligns, so the gap after the index is one byte in the
 * North American, Japanese and European builds and three in the four language
 * builds. */
extern u8 gDebugEffect_bEditedOperand;
extern u8 gDebugEffect_abOperands[2];
extern u8 gDebugEffect_bPage;

#endif
