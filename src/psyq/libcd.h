#ifndef MEMORIES_DECOMP_PSYQ_LIBCD_H
#define MEMORIES_DECOMP_PSYQ_LIBCD_H

#include "../types.h"

#define PSYQ_LIBCD_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef struct {
    u8 minute;
    u8 second;
    u8 sector;
    u8 track;
} CdlLOC;

typedef struct {
    CdlLOC pos;
    s32 size;
    char name[16];
} CdlFILE;

typedef char CdlLOC_size_must_be_4[sizeof(CdlLOC) == 4 ? 1 : -1];
typedef char CdlFILE_size_must_be_24[sizeof(CdlFILE) == 24 ? 1 : -1];
typedef char CdlFILE_size_offset_must_be_4[
    PSYQ_LIBCD_OFFSET(CdlFILE, size) == 4 ? 1 : -1
];
typedef char CdlFILE_name_offset_must_be_8[
    PSYQ_LIBCD_OFFSET(CdlFILE, name) == 8 ? 1 : -1
];

#undef PSYQ_LIBCD_OFFSET

#endif
