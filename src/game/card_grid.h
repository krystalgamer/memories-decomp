#ifndef MEMORIES_DECOMP_CARD_GRID_H
#define MEMORIES_DECOMP_CARD_GRID_H

#include "../types.h"

#define CARD_GRID_SECTION_SIDE_LENGTH 10
#define CARD_GRID_SECTIONS_PER_ROW 2
#define CARD_GRID_SECTION_CARD_COUNT \
    (CARD_GRID_SECTION_SIDE_LENGTH * CARD_GRID_SECTION_SIDE_LENGTH)
#define CARD_GRID_SECTION_ROW_CARD_COUNT \
    (CARD_GRID_SECTION_CARD_COUNT * CARD_GRID_SECTIONS_PER_ROW)

/* The card grid's cursor position.
 *
 * func_8002A788 draws the grid and reads both into s32 locals; func_8002BFCC
 * places the cursor sprite within a section from them, deriving x from the
 * column remainder and y from the row quotient/remainder. The cursor spans
 * two side-by-side 10x10 sections per section row.
 *
 * Both must stay small-data eligible, and that is not a style question here:
 * func_8002A788 reaches them from inline assembly carrying hand-written
 * `.reloc .-4, R_MIPS_GPREL16` directives that name these two symbols, so the
 * assembler has to resolve them gp-relative. A plain s8 scalar is eligible
 * under -G8; an oversized array or a .data attribute would break those
 * relocations rather than merely change a load.
 *
 * The s8 spelling is measured rather than preferred. #3054 established that
 * what this code requires is a signed read: declaring them u8 and dropping
 * the casts builds an executable sixteen bytes short, while u8 with an
 * explicit `(s8)` cast, s8 with the cast, and s8 without it all produce the
 * same instructions. Both sources now take the last of those.
 */
extern s8 gCardGrid_bCursorColumn;
extern s8 gCardGrid_bCursorRow;

#endif
