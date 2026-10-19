#ifndef MEMORIES_DECOMP_LIBRARY_CARD_GRID_H
#define MEMORIES_DECOMP_LIBRARY_CARD_GRID_H

#include "../types.h"
#include "display_object.h"
#include "library_state.h"

/* Draws the Library card-list grid and its cursor box. Never called from C:
 * Library_InitializeScreen (library_runtime.c) and Library_UpdateCardView install its address in
 * D_800E9DB0[3], the per-frame draw callback slot. */
void Library_DrawCardGrid(void);

s32 Library_UpdateCursorMotion(void);
void Library_StartCursorMotion(s32 column, s32 row, s32 frames);

#endif
