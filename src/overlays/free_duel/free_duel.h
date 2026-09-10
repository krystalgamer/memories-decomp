#ifndef MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_H
#define MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_H

#include "../../types.h"

/* Shared state for the free-duel opponent-select screen.
 *
 * The grid is FREE_DUEL_GRID_ROW_COUNT by FREE_DUEL_GRID_COLUMN_COUNT (see
 * card_constants.h). The cursor pair is the committed position; the target
 * pair is where the pad is steering it, clamped to the grid, and the screen
 * runtime walks the cursor toward the target.
 *
 * gFreeDuel_bReturnFlags used to be left out of this header because the
 * resident callers declare it with an explicit .data section attribute that
 * the overlay callers do not use. That is true, and it does not matter here:
 * the question is not whether a divergent spelling exists but whether any file
 * carrying it includes this header. func_8002D458.c and func_80030FA0.c are
 * the two .data declarers and neither includes free_duel.h -- they are
 * resident units and this header is overlay-local -- so the two spellings
 * never meet and the overlay can share the plain one below. fade.h does the
 * same for D_8009B141 and mem_card.h for D_8009B3D4.
 */
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern s8 gFreeDuel_bTargetColumn;
extern s8 gFreeDuel_bTargetRow;

/* One byte per grid cell, non-zero when the cell holds a selectable duelist. */
extern u8 gFreeDuel_abGridAvailable[];

extern u8 gFreeDuel_bScreenFlags;

/* Why the screen ended. screen_runtime.c stores 0x40 and 0x80 at its two exit
 * points and reads the byte back on later passes -- 0x80 once and the whole
 * byte against 0 twice -- so the overlay records the reason and acts on it
 * next time round. The two resident writers only ever clear it. */
extern u8 gFreeDuel_bReturnFlags;

extern void *gFreeDuel_apSparklePool[];

/* The two objects FreeDuel_Init stores here: the thumb it takes from
 * func_800400AC (screen_runtime.c:301, :306) and the cursor from
 * FreeDuel_SpawnSparkle (:307-308). Both are byte pointers.
 * FreeDuel_UpdateScrollbar reads the cursor's y at +0x32 through a
 * FreeDuelWidget view (:93-94, :97, :100) and writes the thumb's y the same
 * way (:102-103). FreeDuel_UpdateCursorTween reaches the cursor at +0x16,
 * +0x30, +0x32, +0x36, +0x38, +0x4C and +0x60 (:364, :378-391, :395-396,
 * :407-410), FreeDuel_UpdateScreen at +8 (:430) and FreeDuel_Entry at +0x44
 * and +0x46 (:520-523). FreeDuelWidget is 0x34 bytes (:48-52), so +0x36,
 * +0x38, +0x44, +0x46, +0x4C and +0x60 lie past it. screen_runtime.c used to
 * declare each twice through asm aliases, `FreeDuelWidget *` and `u8 *`. */
extern u8 *gFreeDuel_pCursorWidget;
extern u8 *gFreeDuel_pThumbWidget;

void FreeDuel_Entry(void);
/* Resident entry alias used before this overlay is linked into its slot. */
void func_80168FB4(void);

#endif
