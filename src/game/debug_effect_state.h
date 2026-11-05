#ifndef MEMORIES_DECOMP_DEBUG_EFFECT_STATE_H
#define MEMORIES_DECOMP_DEBUG_EFFECT_STATE_H

#include "../types.h"

#ifndef DEBUG_EFFECT_COORDINATE_OFFSET
#define DEBUG_EFFECT_COORDINATE_OFFSET 2
#endif
#ifndef DEBUG_EFFECT_PAGE_OFFSET
#define DEBUG_EFFECT_PAGE_OFFSET 4
#endif
#ifndef DEBUG_EFFECT_STATE_SIZE
#define DEBUG_EFFECT_STATE_SIZE 6
#endif

/* The preview screen's state: the operand being edited, the two tuned
 * operands and the page. The Spanish build leaves a larger gap before the
 * operands and no byte after the page. */
typedef struct {
    u8 bEditedOperand;
    u8 pad1[DEBUG_EFFECT_COORDINATE_OFFSET - 1];
    u8 abOperands[2];
    u8 bPage;
#if DEBUG_EFFECT_STATE_SIZE > DEBUG_EFFECT_PAGE_OFFSET + 1
    u8 pad2[DEBUG_EFFECT_STATE_SIZE - DEBUG_EFFECT_PAGE_OFFSET - 1];
#endif
} DebugEffectPreviewState;

typedef char DebugEffectPreviewState_size_must_match[
    sizeof(DebugEffectPreviewState) == DEBUG_EFFECT_STATE_SIZE &&
    DEBUG_EFFECT_PAGE_OFFSET == DEBUG_EFFECT_COORDINATE_OFFSET + 2 ? 1 : -1
];

extern DebugEffectPreviewState gDebugEffect_PreviewState;

#endif
