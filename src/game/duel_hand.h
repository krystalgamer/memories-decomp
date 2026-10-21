#ifndef MEMORIES_DECOMP_DUEL_HAND_H
#define MEMORIES_DECOMP_DUEL_HAND_H

#include "../types.h"
#include "card_constants.h"

struct DisplayObject;

#define DUEL_HAND_SLOT_OFFSET(member) ((u32)&(((DuelHandSlot *)0)->member))

/* One of the HAND_SIZE hand slots at gDuelHand_aSlots.
 *
 * `object` is the card object the draw spawns for the slot; `order_marker` is the
 * sprite DuelHand_CreateSelectionOrderMarker stacks on top of it. Both refer to display-pool slots;
 * consumers retain their context-specific views where needed. `selection_order`
 * is zero for an unmarked slot and holds its one-based order while selecting.
 * Duel_ClearHandSlots zeroes it alongside the two pointers; marker creation
 * publishes the running count, and cancellation renumbers later selections.
 */
typedef struct {
    struct DisplayObject *G32 object;
    struct DisplayObject *G32 order_marker;
    u8 pad_08;
    u8 selection_order;
    u8 pad_0A[2];
} DuelHandSlot;

typedef char DuelHandSlot_object_offset_must_be_0x00[
    DUEL_HAND_SLOT_OFFSET(object) == 0x00 ? 1 : -1
];
typedef char DuelHandSlot_size_must_be_0xC[
    sizeof(DuelHandSlot) == 0xC ? 1 : -1
];
typedef char DuelHandSlot_order_marker_offset_must_be_0x04[
    DUEL_HAND_SLOT_OFFSET(order_marker) == 0x04 ? 1 : -1
];
typedef char DuelHandSlot_selection_order_offset_must_be_0x09[
    DUEL_HAND_SLOT_OFFSET(selection_order) == 0x09 ? 1 : -1
];

#undef DUEL_HAND_SLOT_OFFSET

extern DuelHandSlot gDuelHand_aSlots[HAND_SIZE];

/* How many cards are still to be drawn into those slots. duel_phase_entry.c
 * arms it -- 5 for a full opening hand, and HAND_SIZE - n to top an existing
 * hand back up -- and duel_draw_resolution.c spends it, taking one off per
 * card as the draw animation retires each one and raising bit 0x4000 in the
 * scene state gDuel_wSceneStateFlags once it reaches zero.
 *
 * Declared u8, which is what both sources say, with the one signed read left
 * spelled as a cast where it is:
 *
 *     if (*(s8 *)&gDuel_bPendingDrawCount == 0)
 *
 * That is the same arrangement dialog_choice.h records for
 * gDialog_bInputState: the declaration stays unsigned and the cast at the use
 * is what keeps retail's signed load, so resolving one into the other would
 * be a codegen change rather than a tidy-up. */
extern u8 gDuel_bPendingDrawCount;

void Duel_ClearHandSlots(void);

#endif
