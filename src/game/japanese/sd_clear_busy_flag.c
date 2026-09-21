#define g_SDValue gJapanese_SDValue
#include "../../types.h"
#include "../sound.h"

void SD_ClearBusyFlag(void)
{
    gJapanese_SDValue->busy = 0;
}
