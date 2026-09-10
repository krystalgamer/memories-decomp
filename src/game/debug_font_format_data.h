#ifndef MEMORIES_DECOMP_DEBUG_FONT_FORMAT_DATA_H
#define MEMORIES_DECOMP_DEBUG_FONT_FORMAT_DATA_H

#include "../types.h"

#ifdef DEBUG_FONT_FORMATS_AS_U8_ARRAYS
extern u8 D_8009AF54[];
extern u8 D_8009AF58[];
#else
extern char D_8009AF54[4];
extern char D_8009AF58[4];
#endif

#endif
