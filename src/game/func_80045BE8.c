#include "../types.h"
#include "sound.h"

s32 func_80045BE8(SDCommand *src) {
    SDValue *b1;
    SDValue *b2;
    SDValue *b3;
    s32 n1;
    s32 n2;
    s32 one;

    b1 = g_SDValue;
    n1 = b1->command_count;
    if (n1 >= SD_COMMAND_QUEUE_COUNT) {
        goto full;
    }
    {
        b1->commands[n1].command = src->command;
    }

    b2 = g_SDValue;
    n2 = b2->command_count;
    b2->commands[n2] = *src;

    b3 = g_SDValue;
    one = 1;
    b3->command_count = b3->command_count + one;
    return one;
full:
    return 0;
}
