#include "../../types.h"
#include "../sound.h"

void SD_DropQueuedCdPlayCommands(void)
{
    s32 i;

    for (i = 0; i < g_SDValue->s_wSD_NUM_COMMAND && i >= 0 &&
                g_SDValue->s_wSD_NUM_COMMAND != 0;) {
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
