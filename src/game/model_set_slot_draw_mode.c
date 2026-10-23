#include "../types.h"
#include "model.h"

void Model_SetSlotDrawMode(int index, int value)
{
    gModel_aSlots[index].draw_mode_request = value;
}
