#ifndef MEMORIES_DECOMP_MENU_RECORD_RESET_H
#define MEMORIES_DECOMP_MENU_RECORD_RESET_H

#include "../types.h"

/* Marks every D_800EB010 menu record inactive (effect_id = -1), clears its
   field_32, field_3A, field_3B and child grid, and frees every display-effect
   VRAM slot.
   The returned pointer is a loop leftover that no caller reads. */
s8 *DisplayEffect_ResetMenuRecords(void);

#endif
