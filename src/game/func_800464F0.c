#define G_SDVALUE_AGGREGATE
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
void func_800464F0(void)
{
    s32 i;

    for (i = 0; i < g_SDValue[0]->command_count && i >= 0 &&
                g_SDValue[0]->command_count != 0; ) {
        switch (g_SDValue[0]->commands.c[i].command) {
        case 0x24:
        case 0x29:
        case 0x2B:
            g_SDValue[0]->commands.c[i] =
                ((SDCommandShiftView *)g_SDValue[0])->c[i + 1];
            g_SDValue[0]->command_count--;
            break;
        default:
            i++;
            break;
        }
    }
    switch (g_SDValue[0]->field_007C) {
    case 0x24:
    case 0x29:
    case 0x2B:
        g_SDValue[0]->field_007C = 0;
        g_SDValue[0]->field_007D = 0;
        break;
    }
}
