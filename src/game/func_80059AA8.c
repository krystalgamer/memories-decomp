#include "../types.h"
#include "model.h"
#include "func_80059AA8.h"

int func_80059AA8(int index, int value)
{
    register ModelSlot *entry;
    register int old;

    {
        register ModelSlot *base = D_800F2C40;

        entry = (ModelSlot *)(index * sizeof(ModelSlot));

        entry = (ModelSlot *)((unsigned int)entry + (unsigned int)base);
    }

    old = entry->field_E12;
    if (value >= 0) {
        entry->field_E12 = value;
    }
    return old;
}
