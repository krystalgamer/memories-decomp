#ifndef MEMORIES_DECOMP_DUEL_HAND_H
#define MEMORIES_DECOMP_DUEL_HAND_H

#include "../types.h"
#include "card_constants.h"

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

void Duel_ClearHandSlots(void);

#endif
