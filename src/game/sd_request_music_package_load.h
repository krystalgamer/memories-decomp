#ifndef MEMORIES_DECOMP_SD_REQUEST_MUSIC_PACKAGE_LOAD_H
#define MEMORIES_DECOMP_SD_REQUEST_MUSIC_PACKAGE_LOAD_H

#include "../types.h"

/* One eight-byte entry in the table selected through SDValue.music_table.
 * Only this consumer currently establishes its field layout. */
typedef struct {
    u8 spu_sector_count;
    u8 ram_sector_count;
    u16 sector_offset;
    s32 spu_address;
} SDMusicTableEntry;

typedef char SDMusicTableEntry_size_must_be_8[
    sizeof(SDMusicTableEntry) == 8 ? 1 : -1
];
typedef char SDMusicTableEntry_transfer_counts_must_match[
    (u32)&((SDMusicTableEntry *)0)->spu_sector_count == 0 &&
    (u32)&((SDMusicTableEntry *)0)->ram_sector_count == 1 ? 1 : -1
];
typedef char SDMusicTableEntry_sector_offset_must_be_2[
    (u32)&((SDMusicTableEntry *)0)->sector_offset == 2 ? 1 : -1
];
typedef char SDMusicTableEntry_spu_address_must_be_4[
    (u32)&((SDMusicTableEntry *)0)->spu_address == 4 ? 1 : -1
];

#endif
