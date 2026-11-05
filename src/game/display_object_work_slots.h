#ifndef MEMORIES_DECOMP_DISPLAY_OBJECT_WORK_SLOTS_H
#define MEMORIES_DECOMP_DISPLAY_OBJECT_WORK_SLOTS_H

#include "../types.h"
#include "display_object.h"
#include "duel_check_ritual.h"

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
    DISPLAY_OBJECT_WORK_OFFSET(rgb) == 0x0C ? 1 : -1
];
typedef char DisplayObjectWorkSlots_position_offsets_must_match[
    DISPLAY_OBJECT_WORK_OFFSET(xy.h.x) == 0x30 &&
    DISPLAY_OBJECT_WORK_OFFSET(xy.h.y) == 0x32 ? 1 : -1
];
typedef char DisplayObjectWorkSlots_scale_offsets_must_match[
    DISPLAY_OBJECT_WORK_OFFSET(field_44.scale.x) == 0x44 &&
    DISPLAY_OBJECT_WORK_OFFSET(field_44.scale.y) == 0x46 ? 1 : -1
];
#undef DISPLAY_OBJECT_WORK_OFFSET

/* Shared scratch slots, not an owning object pool. Duel animation uses two,
 * the Exodia presentation uses five, and trade uses the first. The word just
 * past the five slots (+0x14) is a terminator the consumers clear; card
 * placement also reaches it as its sixth selection slot; see
 * notes/card-placement-controller.md. The owner reserves 0x20 bytes in every
 * regional layout, so two more words follow the terminator.
 *
 * Duel_CheckRitual receives base + 8 and writes three pointers followed by
 * zero at base + 0x14 (the terminator word after the five slots).
 * DuelScene_UpdateExodiaResult also clears that word after filling its five
 * slots; see notes/ritual-controller-storage.md. The ritual member overlaps
 * the slots with that complete output subobject rather than casting slot 2
 * to a result that overruns them. */
#define DISPLAY_OBJECT_WORK_STORAGE_WORDS 8

typedef union DisplayObjectWorkSlots {
    DisplayObject *slots[DISPLAY_OBJECT_WORK_STORAGE_WORDS];
    struct {
        DisplayObject *effects[2];
        DuelRitualResult result;
    } ritual;
} DisplayObjectWorkSlots;

typedef char DisplayObjectWorkSlots_size_must_be_0x20[
    sizeof(DisplayObjectWorkSlots) == 0x20 ? 1 : -1
];
typedef char DisplayObjectWorkSlots_result_offset_must_be_8[
    (u32)&((DisplayObjectWorkSlots *)0)->ritual.result == 8 ? 1 : -1
];
extern DisplayObjectWorkSlots gDisplayObject_apWorkSlots;

/* Copies five pointer words and appends zero; the destination ABI is integer. */
void DisplayObject_CopyWorkSlots(s32 *destination);

#endif
