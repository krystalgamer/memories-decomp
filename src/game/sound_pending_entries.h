#ifndef MEMORIES_DECOMP_SOUND_PENDING_ENTRIES_H
#define MEMORIES_DECOMP_SOUND_PENDING_ENTRIES_H

#include "sound.h"

typedef struct {
    s32 count;
    u8 pad0004[4];
    u16 keys[SD_PENDING_INPUT_ENTRY_CAPACITY];
    SDNote data[SD_PENDING_INPUT_ENTRY_CAPACITY];
} SDSeqBlock;

typedef char SDPendingInput_id_size_must_be_halfword[
    SD_PENDING_INPUT_ID_ENTRY_SIZE == sizeof(u16) ? 1 : -1
];
typedef char SDPendingInput_copy_groups_must_cover_block[
    SD_PENDING_INPUT_COPY_GROUP_COUNT * 8 * sizeof(u32) ==
        SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];
typedef char SDSeqBlock_size_must_match_input_block[
    sizeof(SDSeqBlock) == SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];
typedef char SDSeqBlock_id_region_must_have_whole_entries[
    (SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET - SD_PENDING_INPUT_IDS_BYTE_OFFSET) %
        SD_PENDING_INPUT_ID_ENTRY_SIZE == 0 ? 1 : -1
];
typedef char SDSeqBlock_keys_must_match_input_offset[
    (u32)&(((SDSeqBlock *)0)->keys) == SD_PENDING_INPUT_IDS_BYTE_OFFSET ? 1 : -1
];
typedef char SDSeqBlock_data_must_match_input_offset[
    (u32)&(((SDSeqBlock *)0)->data) ==
        SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET ? 1 : -1
];

void func_8004763C(void);

#endif
