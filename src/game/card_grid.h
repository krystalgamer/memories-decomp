#ifndef MEMORIES_DECOMP_CARD_GRID_H
#define MEMORIES_DECOMP_CARD_GRID_H

#include "../types.h"

/* The card grid's cursor position.
 *
 * func_8002A788 draws the grid and reads both into s32 locals; func_8002BFCC
 * places the cursor sprite from them, deriving x from the column as
 * `(c % 10) * 0xE + 0xAE` and y from the row as
 * `(k / 10) * 0xB2 + (k % 10) * 0x10 + 0xE`, so the grid is ten wide.
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
