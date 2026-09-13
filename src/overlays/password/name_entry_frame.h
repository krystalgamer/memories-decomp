#ifndef MEMORIES_DECOMP_NAME_ENTRY_FRAME_H
#define MEMORIES_DECOMP_NAME_ENTRY_FRAME_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

/* The drawing callback's public spelling aliases the central SelectionFrame
   contract used by the keyboard tween; no separate layout is defined here. */

void NameEntry_DrawSelectionFrame(NameEntrySelectionFrameView *frame, GsOT *ot);

#endif
