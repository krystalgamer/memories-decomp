#ifndef MEMORIES_DECOMP_PRIMITIVE_HANDLER_MODES_H
#define MEMORIES_DECOMP_PRIMITIVE_HANDLER_MODES_H

#include "../types.h"

s32 Model_MapToWireframePrimitiveHandler(s32 handler);
s32 Model_MapToBasePrimitiveHandler(s32 handler);

extern u32 gModel_dwSecondPassClutBits;
extern u32 gModel_dwLitColorCacheBase;

#endif
