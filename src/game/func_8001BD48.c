#include "../types.h"

extern signed char gDuel_bOpponentID[];
extern unsigned short gInput_wPad1Pressed[];
extern unsigned char D_8009B164;

int func_8001BD48(void)
{
    if (gDuel_bOpponentID[0] < 0) {
        if (gInput_wPad1Pressed[0] & 0x100) {
            D_8009B164 = 1;
            return 1;
        }
    }
    return 0;
}
