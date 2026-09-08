#ifndef MEMORIES_DECOMP_NAME_ENTRY_FRAME_H
#define MEMORIES_DECOMP_NAME_ENTRY_FRAME_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* Known drawing prefix, not the complete cursor allocation. */
typedef struct {
    u8 pad0[20];
    s16 priority;
    u8 pad16[26];
    s16 x;
    s16 y;
    u8 pad34[8];
    u16 width;
    u16 height;
} NameEntrySelectionFrameView;

void NameEntry_DrawSelectionFrame(NameEntrySelectionFrameView *frame, GsOT *ot);

#endif
