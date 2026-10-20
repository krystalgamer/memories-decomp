#include "../types.h"
#include "script_state.h"

int Script_ReadByte(void)
{
    return *gScript_StreamCursor++;
}

int Script_ReadU16LE(void)
{
    u8 *current = gScript_StreamCursor;

    gScript_StreamCursor = current + 2;
    return current[0] | (current[1] << 8);
}
