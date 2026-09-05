#include "../types.h"

extern u8 gInput_abRawPadBuffers[];
extern u32 gInput_dwPendingHeld;

void Input_ReadRawPads(void)
{
    u8 *p = gInput_abRawPadBuffers;
    if (p[0] == 0 && (p[1] & 0xF) != 0)
        gInput_dwPendingHeld |= ((p[2] << 8) | p[3]) ^ 0xFFFF;
    {
        u8 *q = gInput_abRawPadBuffers;
        if (q[0x22] == 0 && (q[0x23] & 0xF) != 0)
            gInput_dwPendingHeld |=
                ((((q[0x24] << 8) | q[0x25]) ^ 0xFFFF) << 16);
    }
}
