#ifndef MEMORIES_DECOMP_MODEL_REMAP_PRIMITIVE_HANDLERS_H
#define MEMORIES_DECOMP_MODEL_REMAP_PRIMITIVE_HANDLERS_H

#include "../types.h"

/* Fixed prefix of one mapped HMD descriptor. handler_value is the callback
 * word, not the packed registry type key. metadata retains the descriptor
 * word count in its low half and the original count/flags in its high half. */
typedef struct {
    s32 handler_value;
    s32 metadata;
} ModelHandlerRunEntry;

typedef char ModelHandlerRunEntry_prefix_size_must_be_8[
    sizeof(ModelHandlerRunEntry) == 8 ? 1 : -1
];

/* Walks a run of variable-length descriptors, conditionally replacing their
 * callback words. The low halfword at +4 counts words AFTER the leading
 * handler word: stride is (low16 + 1) u32 words, not bytes or a fixed array.
 * The high half remains the raw count/flags gate, not a normalized count.
 *
 * Reverse lookup obtains the type key used for eligibility. The callback
 * takes and returns the stored s32 handler value, not that packed key, an
 * index or a descriptor pointer; only -1 rejects a replacement.
 *
 * arg2 remains the original third argument passed by both callers. The body
 * never reads it; do not remove its call-site setup or infer a new use. */
void Model_RemapPrimitiveHandlers(
    s32 (*fn)(s32),
    ModelHandlerRunEntry *entry,
    s32 arg2,
    s32 count
);

#endif
