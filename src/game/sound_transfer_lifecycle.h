#ifndef MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H
#define MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H

#include "../ygo_types.h"

/* SD_InitState points this at 0x801E1650; func_80045514 fills the request
   passed directly to func_80014C40. Its loads retain the absolute arm. */
#ifdef SOUND_TRANSFER_REQUEST_IN_DATA
extern FileRequestSlot *D_8009B460 __attribute__((section(".data")));
#else
extern FileRequestSlot *D_8009B460;
#endif

/* Embedded pBAV header fields consumed by SD_VabOpenHead. This layout agrees
 * with Psy-Q libsnd's VabHdr without importing that header's unavailable
 * system include dependency into game sources. */
typedef struct {
    s32 form;
    s32 version;
    s32 id;
    u32 file_size;
    u16 reserved_10;
    u16 program_count;
    u16 tone_count;
    u16 vag_count;
    u8 master_volume;
    u8 pan;
    u8 attribute_1;
    u8 attribute_2;
    u32 reserved_1C;
} SDVabHeader;

typedef char SDVabHeader_file_size_offset_must_be_0x0C[
    ((u32)&(((SDVabHeader *)0)->file_size)) == 0x0C ? 1 : -1
];
typedef char SDVabHeader_program_count_offset_must_be_0x12[
    ((u32)&(((SDVabHeader *)0)->program_count)) == 0x12 ? 1 : -1
];
typedef char SDVabHeader_master_volume_offset_must_be_0x18[
    ((u32)&(((SDVabHeader *)0)->master_volume)) == 0x18 ? 1 : -1
];
typedef char SDVabHeader_size_must_be_0x20[
    sizeof(SDVabHeader) == 0x20 ? 1 : -1
];

void func_80049640(void);
void SD_Term(void);
s32 SD_VabOpenHead(SDVabHeader *vab, s16 vab_id, s32 spu_addr);
s32 SD_VabTransBody(s32 value, s16 expected);

#endif
