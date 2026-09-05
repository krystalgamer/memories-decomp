#include "../types.h"
#include "../psyq/libapi.h"

extern u8 gInput_abRawPadBuffers[];
extern void Input_ResetPads(void);
extern u8 D_8009B39C, D_8009B3A2;

void Input_InitPads(void)
{
    InitPAD(
        gInput_abRawPadBuffers,
        0x22,
        gInput_abRawPadBuffers + 0x22,
        0x22
    );
    StartPAD();
    D_8009B39C = 0x18;
    D_8009B3A2 = 0x14;
    Input_ResetPads();
}
