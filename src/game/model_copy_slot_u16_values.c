#include "../types.h"
#include "model_copy_slot_u16_values.h"
#include "model.h"

void Model_CopySlotU16Values(s32 idx, u16 *out) {
    ModelSlot *rec = &D_800F2C40[idx];
    out[0] = rec->field_DD0[0];
    out[1] = rec->field_DD0[1];
    out[2] = rec->field_DD0[2];
    out[3] = rec->field_DD0[3];
}
