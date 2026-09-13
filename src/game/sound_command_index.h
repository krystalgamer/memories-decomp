#ifndef MEMORIES_DECOMP_SOUND_COMMAND_INDEX_H
#define MEMORIES_DECOMP_SOUND_COMMAND_INDEX_H

#include "../types.h"

/* Sixteen four-byte index records precede the payload at 0x50.
   The command pump consumes only each record's low halfword. */
typedef struct {
    u16 field_00;
    u16 count;
    u8 field_04[8];
    s32 payload_size;
    u16 index_words[32];
} CommandIndexTable;

typedef char CommandIndexTable_size_must_be_0x50[
    sizeof(CommandIndexTable) == 0x50 ? 1 : -1
];
typedef char CommandIndexTable_indices_must_be_at_0x10[
    (u32)&((CommandIndexTable *)0)->index_words == 0x10 ? 1 : -1
];

#endif
