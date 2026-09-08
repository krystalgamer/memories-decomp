#ifndef MEMORIES_DECOMP_YGO_TYPES_H
#define MEMORIES_DECOMP_YGO_TYPES_H

#include "types.h"

typedef struct {
    s16 id;
    u16 count;
} CardCountEntry;

typedef char CardCountEntry_size_must_be_4[
    sizeof(CardCountEntry) == 4 ? 1 : -1
];

typedef struct {
    s32 key;
    void **handler;
} ModelHandlerObject;

#endif
