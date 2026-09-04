#include "../../types.h"

extern void *gFreeDuel_apSparklePool[];

void **FreeDuel_GetSparkleSlot(void)
{
    s32 i;

    for (i = 15; i >= 0; i--) {
        if (gFreeDuel_apSparklePool[i] == 0) {
            return &gFreeDuel_apSparklePool[i];
        }
    }
    return 0;
}
