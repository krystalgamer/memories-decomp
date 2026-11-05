#include "../types.h"
#include "script_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 *gScript_StreamCursor;

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
