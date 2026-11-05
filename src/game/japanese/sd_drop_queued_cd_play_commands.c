#include "../../types.h"
#include "../sound.h"

void SD_DropQueuedCdPlayCommands(void)
{
    s32 i;

    for (i = 0; i < g_SDValue->command_count && i >= 0 &&
                g_SDValue->command_count != 0;) {
        switch (g_SDValue->commands.c[i].command) {
        case 0x24:
        case 0x29:
        case 0x2B:
            g_SDValue->commands.c[i] =
                ((SDCommandShiftView *)g_SDValue)->c[i + 1];
            g_SDValue->command_count--;
            break;
        default:
            i++;
            break;
        }
    }
    switch (g_SDValue->active_command) {
    case 0x24:
    case 0x29:
    case 0x2B:
        g_SDValue->active_command = 0;
        g_SDValue->active_command_phase = 0;
        break;
    }
}
