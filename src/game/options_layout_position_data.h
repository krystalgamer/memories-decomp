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
 * Declared here with an unknown bound, the usual C spelling for an array
 * defined elsewhere; options_layout_position_data.c defines it with its four
 * halfwords, so the owner keeps it in small data while Options_UpdateLayout
 * reaches it through cc1psx's split %hi/%lo pair, as retail does. */
extern u16 gOptions_SelectionRowPositions[];

#endif
