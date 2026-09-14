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
 * default incomplete array: Main_RunTrade needs split absolute addressing
 * at -G8. Card placement additionally reaches the independent word at +0x14
 * (D_800E9F04) as its sixth selection slot; see
 * notes/card-placement-controller.md. The ritual view below does not change
 * the five-slot count used by the other consumers. */
#ifdef DISPLAY_OBJECT_WORK_RITUAL_VIEW
#include "duel_check_ritual.h"

/* Duel_CheckRitual receives base + 8 and writes three pointers followed by
 * zero at base + 0x14 (the independent D_800E9F04 label). func_80018FEC
 * also clears that word after filling its five slots. The resident image
 * backs this complete 0x18-byte range; see notes/ritual-controller-storage.md.
 * Overlap the five-slot view with the actual, complete output subobject
 * rather than casting slot 2 to a result that overruns a five-pointer array. */
typedef union {
    DisplayObject *slots[DISPLAY_OBJECT_WORK_SLOT_COUNT];
    struct {
        DisplayObject *effects[2];
        DuelRitualResult result;
    } ritual;
} DisplayObjectRitualWorkArea;

typedef char DisplayObjectRitualWorkArea_size_must_be_0x18[
    sizeof(DisplayObjectRitualWorkArea) == 0x18 ? 1 : -1
];
typedef char DisplayObjectRitualWorkArea_result_offset_must_be_8[
    (u32)&((DisplayObjectRitualWorkArea *)0)->ritual.result == 8 ? 1 : -1
];
extern DisplayObjectRitualWorkArea D_800E9EF0;
#else
extern DisplayObject *D_800E9EF0[];
#endif

/* Copies five pointer words and appends zero; the destination ABI is integer. */
void func_8002CB50(s32 *destination);

#endif
