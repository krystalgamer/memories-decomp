#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_WORK_SLOTS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_WORK_SLOTS_H

#include "../types.h"
#include "display_object.h"

#define DISPLAY_OBJECT_WORK_SLOT_COUNT 5

typedef char DisplayObjectWorkSlots_span_must_be_0x14[
    sizeof(DisplayObject *) * DISPLAY_OBJECT_WORK_SLOT_COUNT == 0x14 ? 1 : -1
];

#define DISPLAY_OBJECT_WORK_OFFSET(member) ((u32)&((DisplayObject *)0)->member)
typedef char DisplayObjectWorkSlots_attribute_offset_must_be_0x04[
    DISPLAY_OBJECT_WORK_OFFSET(attribute) == 0x04 ? 1 : -1
];
typedef char DisplayObjectWorkSlots_flags_offset_must_be_0x08[
    DISPLAY_OBJECT_WORK_OFFSET(flags) == 0x08 ? 1 : -1
];
typedef char DisplayObjectWorkSlots_colour_offset_must_be_0x0C[
    DISPLAY_OBJECT_WORK_OFFSET(field_0C) == 0x0C ? 1 : -1
];
typedef char DisplayObjectWorkSlots_position_offsets_must_match[
    DISPLAY_OBJECT_WORK_OFFSET(field_30.h.field_30) == 0x30 &&
    DISPLAY_OBJECT_WORK_OFFSET(field_30.h.field_32) == 0x32 ? 1 : -1
];
typedef char DisplayObjectWorkSlots_scale_offsets_must_match[
    DISPLAY_OBJECT_WORK_OFFSET(field_44.h.field_44) == 0x44 &&
    DISPLAY_OBJECT_WORK_OFFSET(field_44.h.field_46) == 0x46 ? 1 : -1
];
#undef DISPLAY_OBJECT_WORK_OFFSET

/* Shared scratch slots, not an owning object pool. Duel animation uses two,
 * the Exodia presentation uses five, and trade uses the first. Keep the
 * incomplete array: Main_RunTrade needs split absolute addressing at -G8. */
extern DisplayObject *D_800E9EF0[];

/* Copies five pointer words and appends zero; the destination ABI is integer. */
void func_8002CB50(s32 *destination);

#endif
