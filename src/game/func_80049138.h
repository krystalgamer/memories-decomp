#ifndef MEMORIES_DECOMP_FUNC_80049138_H
#define MEMORIES_DECOMP_FUNC_80049138_H

#include "../types.h"

/* One eight-byte entry in the table selected through SDValue.field_1560.
 * Only this consumer currently establishes its field layout. */
typedef struct {
    u8 field_00;
    u8 field_01;
    u16 field_02;
    s32 field_04;
} SDMusicTableEntry;

typedef char SDMusicTableEntry_size_must_be_8[
    sizeof(SDMusicTableEntry) == 8 ? 1 : -1
];

#endif
