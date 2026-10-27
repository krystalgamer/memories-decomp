#ifndef MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H
#define MEMORIES_DECOMP_SOUND_TRANSFER_LIFECYCLE_H

#include "../ygo_types.h"

/* SD_InitState points this at 0x801E1650; SD_StepActiveCommand fills the request
   passed directly to File_RequestSecondarySlot. Its loads retain the absolute arm. */
#ifdef SOUND_TRANSFER_REQUEST_IN_DATA
extern FileRequestSlot *G32 gSD_pFileRequest __attribute__((section(".data")));
#else
extern FileRequestSlot *G32 gSD_pFileRequest;
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

/* Table offsets inside a loaded VAB header image. */
#define SD_VAB_PROGRAM_TABLE_OFFSET 0x20
#define SD_VAB_TONE_TABLE_OFFSET 0x820
#define SD_VAB_TONES_PER_PROGRAM 16

/* The 16-byte program record (libsnd ProgAtr). A VAB image holds 128 of
   them at +0x20, straight after SDVabHeader. */
typedef struct {
    u8 tone_count;
    u8 volume;
    u8 priority;
    u8 mode;
    u8 pan;
    u8 reserved_05;
    s16 attribute;
    u32 reserved_08;
    u32 reserved_0C;
} SDVabProgram;

/* The 32-byte tone record (libsnd VagAtr). Each program owns sixteen of them
   in the table at +0x820; the VAG size table follows that table. */
typedef struct {
    u8 priority;
    u8 mode;
    u8 volume;
    u8 pan;
    u8 center_note;
    u8 center_fine;
    u8 note_min;
    u8 note_max;
    u8 vibrato_width;
    u8 vibrato_time;
    u8 portamento_width;
    u8 portamento_time;
    u8 pitch_bend_min;
    u8 pitch_bend_max;
    u8 reserved_0E;
    u8 reserved_0F;
    u16 adsr1;
    u16 adsr2;
    s16 program;
    s16 vag;
    s16 reserved_18[4];
} SDVabTone;

typedef char SDVabProgram_size_must_be_0x10[
    sizeof(SDVabProgram) == 0x10 ? 1 : -1
];
typedef char SDVabTone_adsr1_offset_must_be_0x10[
    ((u32)&(((SDVabTone *)0)->adsr1)) == 0x10 ? 1 : -1
];
typedef char SDVabTone_vag_offset_must_be_0x16[
    ((u32)&(((SDVabTone *)0)->vag)) == 0x16 ? 1 : -1
];
typedef char SDVabTone_size_must_be_0x20[
    sizeof(SDVabTone) == 0x20 ? 1 : -1
];

void SD_ResetSecondaryRuntime(void);
void SD_Term(void);
s32 SD_VabOpenHead(SDVabHeader *vab, s16 vab_id, s32 spu_addr);
s32 SD_VabTransBody(s32 source_address, s16 expected_token);

#endif
