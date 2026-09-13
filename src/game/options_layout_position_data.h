#ifndef MEMORIES_DECOMP_OPTIONS_LAYOUT_POSITION_DATA_H
#define MEMORIES_DECOMP_OPTIONS_LAYOUT_POSITION_DATA_H

#include "../ygo_types.h"

/* The byte-array arm is an addressing form, not a second type, and two
 * measurements say it has to stay. options_screen.c is the only unit that
 * selects it, for one `*(OptionsLayoutPositionBlock *)` copy, and retail
 * reaches the symbol there through cc1psx's own split pair -- `lui $v0,
 * %hi(D_8009AF5C)` then `addiu $t2, $v0, %lo(D_8009AF5C)`, two registers, so
 * the halves can be scheduled apart.
 *
 *   - With the sized declaration and `&D_8009AF5C`: `sizeof` is 8 (asserted
 *     in ygo_types.h) and the -G8 threshold is `<=`, so the symbol is small
 *     data and one gp-relative instruction. The executable comes out
 *     0x1D07FC bytes against 0x1D0800, one instruction short.
 *   - With the true type in `.data` and `&D_8009AF5C`, which is how
 *     graphics_frame.h takes a symbol out of small data: the length is right
 *     and the bytes are not, differing at VRAM 0x8003C56C -- that arm gives
 *     the assembler's adjacent bare form where retail has the split pair.
 *
 * An incomplete array is what produces cc1psx's pair, and no spelling at the
 * use site substitutes for it. */
#ifdef D_8009AF5C_AS_BYTE_ARRAY
extern u8 D_8009AF5C[];
#else
extern OptionsLayoutPositionData D_8009AF5C;
#endif

#endif
