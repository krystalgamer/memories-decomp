#include "../types.h"
#include "model.h"

/* This is D_800F2C40[0].field_D70, but spelling the base that way materializes
   the 0xD70 offset and makes this function one instruction longer. */
extern u8 D_800F39B0[];

unsigned char *func_800591C0(unsigned int index, unsigned int slot)
{
    register u8 *entry;

    if (slot >= 3) {
        slot = 0;
    }
    entry = D_800F39B0 + index * MODEL_SLOT_SIZE;

    return entry + slot * sizeof(ModelSlotLightEntry);
}
