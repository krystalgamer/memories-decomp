#include "../../types.h"
#include "helpers.h"

void func_80168004(s32 selection)
{
    u8 *colors = gText_abColorSlots;

    colors[0] = 4;
    colors[1] = 4;
    colors[2] = 4;
    colors[3] = 4;
    colors[4] = 4;
    colors[selection] = 0;
    if (selection != 0) {
        colors[3] = 2;
    } else {
        colors[4] = 2;
    }
}
