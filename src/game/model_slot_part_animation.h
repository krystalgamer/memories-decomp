#ifndef MEMORIES_DECOMP_MODEL_SLOT_PART_ANIMATION_H
#define MEMORIES_DECOMP_MODEL_SLOT_PART_ANIMATION_H

#include "../types.h"
#include "model.h"

/* Reads back where a slot part stands, roughly inverting
   Model_SeekSlotPartAnimation. It returns 0 if the part is missing or its
   current command (ti) is negative. Otherwise it finds the first sequence row
   animation_row in 1..9 whose entry index is not 0xFFFF and whose header entry (the one
   before the row's first entry) is at or before ti, with ti no further than
   the header's halfword counted from the table base. It sums byte 2 of the
   row's entries before ti and adds (tframe - rframe) / 16 unless rframe holds
   one of the dispatcher's 0x6000/0x7000 overrun markers. The result packs animation_row
   into the high byte above that sum, or is 0 when no row matches. No caller
   was found in SLUS_014.11, SU.MRG or WA_MRG.MRG. */
s32 tent_Model_GetSlotPartAnimationPosition(ModelSlot *slot, s32 part_index);

/* Seeks channel part_index of a model slot to position scaled_position within sequence animation_row.
 *
 * An earlier note here said the definition could not take a ModelSlot,
 * because introducing the cast changed the entry instruction. That was the
 * member spelling, not the type. `(u8 *)slot->animation_command_indices + offset` reassociates
 * so that the slot pointer becomes the second operand of the addu, and
 * retail has it first. A plain `slot->animation_command_indices[animation_row][part_index]` index keeps it first, as
 * long as the two channel offsets are still computed up front for cse to
 * reuse. The definition takes the typed slot and all three callers pass it
 * directly. */
void Model_SeekSlotPartAnimation(ModelSlot *slot, s32 part_index, s32 animation_row, u32 scaled_position);

#endif
