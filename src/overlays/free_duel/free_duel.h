#ifndef MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_H
#define MEMORIES_DECOMP_OVERLAYS_FREE_DUEL_H

#include "../../types.h"
#include "../../game/display_object.h"
#include "module_state.h"

/* Shared state for the free-duel opponent-select screen.
 *
 * The grid is FREE_DUEL_GRID_ROW_COUNT by FREE_DUEL_GRID_COLUMN_COUNT (see
 * card_constants.h). The cursor pair is the committed position; the target
 * pair is where the pad is steering it, clamped to the grid, and the screen
 * runtime walks the cursor toward the target.
 *
 * gFreeDuel_bReturnFlags has two spellings. The overlay reaches it through
 * the plain arm below; the resident writers main_apply_menu_selection.c and
 * debug_menu_leave_entries.c address it with %hi/%lo and define
 * GFREEDUEL_BRETURNFLAGS_IN_DATA to take the .data arm, the same shape fade.h
 * uses for D_8009B141.
 */
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern s8 gFreeDuel_bTargetColumn;
extern s8 gFreeDuel_bTargetRow;

/* Why the screen ended. screen_runtime.c stores 0x40 and 0x80 at its two exit
 * points and reads the byte back on later passes -- 0x80 once and the whole
 * byte against 0 twice -- so the overlay records the reason and acts on it
 * next time round. The two resident writers only ever clear it. */
#ifdef GFREEDUEL_BRETURNFLAGS_IN_DATA
extern u8 gFreeDuel_bReturnFlags __attribute__((section(".data")));
#else
extern u8 gFreeDuel_bReturnFlags;
#endif

/* Builds the duelist grid screen, uploading each available duelist's
 * portrait record from `src` to VRAM with LoadImage2 on the way.
 * Main_InitFreeDuelMenu calls it from resident code once the module transfer
 * has finished, and c_symbols.ld gives the resident link its address. */
void FreeDuel_Init(u8 *src);

void FreeDuel_Entry(void);
/* Resident entry alias used before this overlay is linked into its slot. */
void func_80168FB4(void);

#endif
