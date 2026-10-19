#include "../types.h"
#include "duel_side_view_angles.h"

/* Small data at 0x8009AF20: opposing headings for the two duel sides.
   Field transitions and row movement feed the view-tween heading channel;
   Duel_InitGroundProjectionReference samples element 0 directly. */
u16 gDuel_awSideViewHeadings[2] __attribute__((section(".sdata"))) = {
    0x0400,
    0x0C00,
};
