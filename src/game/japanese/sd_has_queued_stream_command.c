#define g_SDValue gJapanese_SDValue
#include "../../types.h"
#include "../sound.h"

s32 SD_HasQueuedStreamCommand(void)
{
    s32 i;

    for (i = 1; i < gJapanese_SDValue->command_count; i++) {
        s32 c = gJapanese_SDValue->commands.c[i].command;

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
