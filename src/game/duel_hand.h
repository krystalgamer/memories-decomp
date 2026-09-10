#ifndef MEMORIES_DECOMP_DUEL_HAND_H
#define MEMORIES_DECOMP_DUEL_HAND_H

#include "../types.h"
#include "card_constants.h"

struct DisplayObject;

#define DUEL_HAND_SLOT_OFFSET(member) ((u32)&(((DuelHandSlot *)0)->member))

/* One of the HAND_SIZE hand slots at D_800EA030.
 *
 * `object` is the card object the draw spawns for the slot; `child` is the
 * sprite func_8001B7AC stacks on top of it. Both are display objects, held as
 * bytes because their callers describe them differently. `active_09` is the
 * byte Duel_ClearHandSlots zeroes alongside the two pointers: func_8001B8B8
 * dims every slot whose byte is zero, and func_8001B7AC stores the object's
 * running count in it.
 */
typedef struct {
    u8 *object;
    u8 *child;
    u8 pad_08;
    u8 active_09;
    u8 pad_0A[2];
} DuelHandSlot;

typedef char DuelHandSlot_size_must_be_0xC[
    sizeof(DuelHandSlot) == 0xC ? 1 : -1
];
typedef char DuelHandSlot_child_offset_must_be_0x04[
    DUEL_HAND_SLOT_OFFSET(child) == 0x04 ? 1 : -1
];
typedef char DuelHandSlot_active_09_offset_must_be_0x09[
    DUEL_HAND_SLOT_OFFSET(active_09) == 0x09 ? 1 : -1
];

#undef DUEL_HAND_SLOT_OFFSET

extern DuelHandSlot D_800EA030[HAND_SIZE];

/* How many cards are still to be drawn into those slots. duel_phase_entry.c
 * arms it -- 5 for a full opening hand, and HAND_SIZE - n to top an existing
 * hand back up -- and duel_draw_resolution.c spends it, taking one off per
 * card as the draw animation retires each one and raising bit 0x4000 in the
 * scene state D_8009B23A once it reaches zero.
 *
 * Declared u8, which is what both sources say, with the one signed read left
 * spelled as a cast where it is:
 *
 *     if (*(s8 *)&D_8009B1EC == 0)
 *
 * That is the same arrangement dialog_choice.h records for
 * gDialog_bInputState: the declaration stays unsigned and the cast at the use
 * is what keeps retail's signed load, so resolving one into the other would
 * be a codegen change rather than a tidy-up. */
extern u8 D_8009B1EC;

/* State shared by the two adjacent hand-stack callbacks. func_8001B780
 * positions `position_object` from the selected slot, and func_8001B7AC uses
 * the same slot index while advancing the running child count. */
typedef struct {
    u8 pad_00[4];
    struct DisplayObject *position_object;
    u8 pad_08[6];
    s8 slot_index;
    u8 pad_0F[6];
    u8 count;
    u8 pad_16[2];
} DuelHandStackState;

#define DUEL_HAND_STACK_STATE_OFFSET(member) \
    ((u32)&(((DuelHandStackState *)0)->member))

typedef char DuelHandStackState_position_object_offset_must_be_0x4[
    DUEL_HAND_STACK_STATE_OFFSET(position_object) == 0x4 ? 1 : -1
];
typedef char DuelHandStackState_slot_index_offset_must_be_0xE[
    DUEL_HAND_STACK_STATE_OFFSET(slot_index) == 0xE ? 1 : -1
];
typedef char DuelHandStackState_count_offset_must_be_0x15[
    DUEL_HAND_STACK_STATE_OFFSET(count) == 0x15 ? 1 : -1
];
typedef char DuelHandStackState_size_must_be_0x18[
    sizeof(DuelHandStackState) == 0x18 ? 1 : -1
];

#undef DUEL_HAND_STACK_STATE_OFFSET

void Duel_ClearHandSlots(void);

#endif
