#define G_SDVALUE_AGGREGATE
#include "../../types.h"
#include "../sound.h"

void SD_DropQueuedCdPlayCommands(void)
{
    s32 i;

    for (i = 0; i < g_SDValue[0]->command_count && i >= 0 &&
                g_SDValue[0]->command_count != 0;) {
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
    switch (g_SDValue[0]->active_command) {
    case 0x24:
    case 0x29:
    case 0x2B:
        g_SDValue[0]->active_command = 0;
        g_SDValue[0]->active_command_phase = 0;
        break;
    }
}
