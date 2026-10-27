#include "../types.h"
#include "model_handler_registry.h"
#include "model_remap_primitive_handlers.h"

void Model_RemapPrimitiveHandlers(s32 (*remap_handler)(s32), ModelHandlerRunEntry *descriptor, s32 arg2,
                   s32 descriptor_count)
{
    s32 remaining_descriptors;
    s32 type_key;
    s32 count_flags_word;
    s32 type_tag;
    s32 descriptor_stride_words;
    s32 replacement_handler;

    for (remaining_descriptors = descriptor_count - 1; remaining_descriptors != -1; remaining_descriptors--) {
        type_key = Model_FindHandlerKey(descriptor->handler_value);
        count_flags_word = descriptor->metadata;
        type_tag = type_key >> 24;
        descriptor_stride_words = (count_flags_word & 0xFFFF) + 1;
        count_flags_word &= 0xFFFF0000;
        if (((u32)type_tag < 2 && (type_key & 0xFFFF) != 0 && count_flags_word != 0) ||
            (type_tag == 1 && (type_key & 0xFFFF) == 0)) {
            replacement_handler = remap_handler(descriptor->handler_value);
            if (replacement_handler != -1) {
                descriptor->handler_value = replacement_handler;
            }
        }
        descriptor = (ModelHandlerRunEntry *)&((u32 *)descriptor)[descriptor_stride_words];
    }
}
