#include "../../types.h"
#include "../sound.h"

s32 SD_HasQueuedStreamCommand(void)
{
    s32 i;

    for (i = 1; i < g_SDValue->s_wSD_NUM_COMMAND; i++) {
        s32 c = g_SDValue->s_uSD_COMMAND_QUEUE.c[i].command;

        if (c == 0x20) {
            goto found;
        }
        if (c < 0x21) {
            if (c == 0x11) {
                goto found;
            }
            continue;
        } else if (c != 0x24) {
            continue;
        }
    found:
        return 1;
    }
    return 0;
}
