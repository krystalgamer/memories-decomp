#include "../types.h"
#include "duel_side_view_angles.h"

/* Small data at 0x8009AF20: the opposing view angle for each duel side.
   Duel_InitGroundProjectionReference samples the reference from element 0, while
   DuelCursor_ProcessGridDirection indexes the pair by the active side during row movement. */
u16 D_8009AF20[2] __attribute__((section(".sdata"))) = {
    0x0400,
    0x0C00,
};
