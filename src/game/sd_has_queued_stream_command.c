#include "../types.h"
#include "sound.h"

/* Scans entries [1, count) for one carrying a space (0x20), the 0x11
   CD-track marker, or a '$' (0x24) in f0; returns 1 on the first hit. */
s32 SD_HasQueuedStreamCommand(void) {
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
        } else {
            if (c != 0x24) {
                continue;
            }
        }
    found:
        return 1;
    }
    return 0;
}
