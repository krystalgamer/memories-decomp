#ifndef MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H
#define MEMORIES_DECOMP_CARD_PREVIEW_CALLBACKS_H

#include "../types.h"
#include "display_object.h"

typedef struct {
    u8 pad_00[0x6A];
    u8 variant;
    u8 pad_6B[0x10F];
} CardVariantSource;

typedef char CardVariantSource_variant_offset_must_be_0x6A[
    (u32)&(((CardVariantSource *)0)->variant) == 0x6A ? 1 : -1
];
typedef char CardVariantSource_size_must_be_0x17A[
    sizeof(CardVariantSource) == 0x17A ? 1 : -1
];

void func_80028B08(DisplayObject *object, s32 arg1);
/* Applies the preview object's stored card variant, or fallback variant 2,
 * then redraws and requests the current display-list pass to rerun. */
void CardPreview_UpdateVariant(CardVariantSource *source, s32 arg1);

#endif
