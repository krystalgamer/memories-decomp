#ifndef MEMORIES_DECOMP_OPTIONS_LAYOUT_POSITION_DATA_H
#define MEMORIES_DECOMP_OPTIONS_LAYOUT_POSITION_DATA_H

#include "../ygo_types.h"

typedef struct {
    OptionsLayoutPositionBlock selection;
    u16 pad_06;
    u16 output_x[2];
} OptionsLayoutBuffer;

typedef char OptionsLayoutBuffer_size_must_be_12[
    sizeof(OptionsLayoutBuffer) == 12 ? 1 : -1
];

/* gOptions_SelectionRowPositions holds the selection cursor's row Y values
 * {0xFFFF, 0x0090, 0x00A0}: Options_UpdateLayout copies the block to the
 * stack and places gOptions_pSelectionCursor at positions[selection] + 8
 * (row 0 instead follows gOptions_pOutputCursor, so its 0xFFFF is unused
 * there).
 *
 * The byte-array arm is an addressing form, not a second type, and two
 * measurements say it has to stay. options_screen.c is the only unit that
 * selects it, for one `*(OptionsLayoutPositionBlock *)` copy, and retail
 * reaches the symbol there through cc1psx's own split pair -- `lui $v0,
 * %hi(gOptions_SelectionRowPositions)` then `addiu $t2, $v0, %lo(gOptions_SelectionRowPositions)`, two registers, so
 * the halves can be scheduled apart.
 *
 *   - With the sized declaration and `&gOptions_SelectionRowPositions`: `sizeof` is 8 (asserted
 *     in ygo_types.h) and the -G8 threshold is `<=`, so the symbol is small
 *     data and one gp-relative instruction. The executable comes out
 *     0x1D07FC bytes against 0x1D0800, one instruction short.
 *   - With the true type in `.data` and `&gOptions_SelectionRowPositions`, which is how
 *     graphics_frame.h takes a symbol out of small data: the length is right
 *     and the bytes are not, differing at VRAM 0x8003C56C -- that arm gives
 *     the assembler's adjacent bare form where retail has the split pair.
 *
 * An incomplete array is what produces cc1psx's pair, and no spelling at the
 * use site substitutes for it. */
#ifdef VERSION_JAPAN
#define OPTIONS_SELECTION_ROW_POSITIONS_LABEL asm("gJapanese_OptionsLayoutPositions")
#else
#define OPTIONS_SELECTION_ROW_POSITIONS_LABEL
#endif
#ifdef D_8009AF5C_AS_BYTE_ARRAY
extern u8 gOptions_SelectionRowPositions[] OPTIONS_SELECTION_ROW_POSITIONS_LABEL;
#else
extern OptionsLayoutPositionData gOptions_SelectionRowPositions
    OPTIONS_SELECTION_ROW_POSITIONS_LABEL;
#endif

#endif
