#ifndef MEMORIES_DECOMP_MODEL_SLOT_QUERIES_H
#define MEMORIES_DECOMP_MODEL_SLOT_QUERIES_H

#include "../types.h"

/* Three reads of one model slot, each selected by slot index rather than by a
 * slot pointer, so a caller needs nothing of the record's layout.
 *
 * func_80058E94 returns its field shifted right by four, so the value handed
 * back is not the byte stored at 0xE06.
 *
 * func_80058E3C and func_80058E94 have no caller yet -- none in C and none in
 * the generated assembly. They are declared here anyway so that whoever
 * decompiles their callers finds a signature instead of inventing one. */
s32 func_80058E3C(s32 index);
s32 func_80058E68(s32 index);
s32 func_80058E94(s32 index);

#endif
