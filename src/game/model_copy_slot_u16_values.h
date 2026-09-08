#ifndef MEMORIES_DECOMP_MODEL_COPY_SLOT_U16_VALUES_H
#define MEMORIES_DECOMP_MODEL_COPY_SLOT_U16_VALUES_H

#include "../types.h"

/* Copies the four halfwords at field_DD0 of model slot idx into out. */
void Model_CopySlotU16Values(s32 idx, u16 *out);

#endif
