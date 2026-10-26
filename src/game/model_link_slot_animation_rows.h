#ifndef MEMORIES_DECOMP_MODEL_LINK_SLOT_ANIMATION_ROWS_H
#define MEMORIES_DECOMP_MODEL_LINK_SLOT_ANIMATION_ROWS_H

#include "../types.h"

/* Relinks the command list of one model slot's animation row tables.
 *
 * It returns immediately unless the slot has both a command list in
 * animation_commands and a nonzero channel count in animation_sequence_count, then walks that
 * count of channels by MODEL_SLOT_ROW_COUNT rows, rewriting halfword links
 * in the list so entries point at one another rather than at a key.
 *
 * model_slot_row_tables.c describes the family this belongs to: the reset
 * fills the keys with 0xFFFF and imports the command list in animation_commands.
 * Model_BuildSlotAnimationTables then claims the keys and accumulates row
 * durations. This relink pass follows it in model-load phase 3. */
void Model_LinkSlotAnimationRows(s32 slot_index);

#endif
