#ifndef MEMORIES_DECOMP_FUNC_8002BAB4_H
#define MEMORIES_DECOMP_FUNC_8002BAB4_H

#include "../types.h"

typedef struct {
    s32 values[4];
} LibraryViewQuad;

typedef char LibraryViewQuad_size_must_be_0x10[
    sizeof(LibraryViewQuad) == 0x10 ? 1 : -1
];

void func_8002BAB4(void);

#endif
