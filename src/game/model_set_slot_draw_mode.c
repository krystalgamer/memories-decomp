#include "../types.h"
#include "model.h"

void Model_SetSlotDrawMode(int index, int value)
{
    gModel_aSlots[index].field_E11 = value;
}
