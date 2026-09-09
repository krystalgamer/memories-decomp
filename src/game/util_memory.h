#ifndef UTIL_MEMORY_H
#define UTIL_MEMORY_H

#include "../types.h"

void Util_CopyWords(u8 *destination, u8 *source, u32 length);
void Util_FillMemory(u8 *destination, s32 value, u32 length);

#endif
