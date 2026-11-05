#include "../types.h"
#include "sound.h"

/* Drops every queued 0x24, 0x29 and 0x2B command, shifting the next record
 * down over each one and shortening the queue, then clears the pending
 * 0x7C/0x7D pair when it holds one of those commands.
 *
 * The queue walk tests the count, the index sign and an empty queue in that
 * order: GCC rotates the three-part condition so its entry check is only
 * count > 0 and the back edge re-enters at the empty-queue test, which is
 * the retail loop shape. The switches give the retail 0x29-first compare
 * tree. */
void SD_DropQueuedCdPlayCommands(void)
{
    s32 i;

    for (i = 0; i < g_SDValue->s_wSD_NUM_COMMAND && i >= 0 &&
                g_SDValue->s_wSD_NUM_COMMAND != 0; ) {
        switch (g_SDValue->s_uSD_COMMAND_QUEUE.c[i].command) {
        case 0x24:
        case 0x29:
        case 0x2B:
            g_SDValue->s_uSD_COMMAND_QUEUE.c[i] =
                ((SDCommandShiftView *)g_SDValue)->c[i + 1];
            g_SDValue->s_wSD_NUM_COMMAND--;
            break;
        default:
            i++;
            break;
        }
    }
    switch (g_SDValue->s_bSD_ACTIVE_COMMAND) {
    case 0x24:
    case 0x29:
    case 0x2B:
        g_SDValue->s_bSD_ACTIVE_COMMAND = 0;
        g_SDValue->s_bSD_ACTIVE_COMMAND_PHASE = 0;
        break;
    }
}
