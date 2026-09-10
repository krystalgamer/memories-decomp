#ifndef MEMORIES_DECOMP_MODEL_SLOT_DATA_H
#define MEMORIES_DECOMP_MODEL_SLOT_DATA_H

#include "../types.h"

/* Address of one MODEL_SLOT_DATA_ENTRY_SIZE entry of model slot `index`'s data.
 * `slot` beyond the slot's entry_count falls back to its current entry,
 * field_E18. */
void *func_80058F20(s32 index, s32 slot);

/* The same address for the current entry, returned as an integer. The only
 * caller, the campaign map's set_location.c, casts it back to a pointer and
 * hands it to func_8005922C as the slot's coordinate unit. */
u32 func_80058F74(s32 index);

#endif
