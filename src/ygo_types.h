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

#define YGO_TYPE_OFFSET(type, member) ((u32)&(((type *)0)->member))

typedef struct FileTransferDescriptor FileTransferDescriptor;
typedef void (*FileTransferCallback)();

struct FileTransferDescriptor {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u32 value_08;
    u32 value_0C;
    volatile s32 total_bytes;
    s32 file_bytes;
    u8 *loader_argument;
    u32 mode;
    FileTransferCallback phase_callback;
    s32 absolute_lba;
    s32 phase_remaining;
    u32 status_flags;
    u16 counter;
    u16 field_32;
    s32 direct_destination;
    void *callback_data;
    u32 position;
    u32 result;
    u16 buffer_index;
    u8 done;
    u8 substate;
};

typedef char FileTransferDescriptor_size_must_be_0x48[
    sizeof(FileTransferDescriptor) == 0x48 ? 1 : -1
];
typedef char FileTransferDescriptor_w_offset_must_be_0x04[
    YGO_TYPE_OFFSET(FileTransferDescriptor, w) == 0x04 ? 1 : -1
];
typedef char FileTransferDescriptor_h_offset_must_be_0x06[
    YGO_TYPE_OFFSET(FileTransferDescriptor, h) == 0x06 ? 1 : -1
];
typedef char FileTransferDescriptor_value_08_offset_must_be_0x08[
    YGO_TYPE_OFFSET(FileTransferDescriptor, value_08) == 0x08 ? 1 : -1
];
typedef char FileTransferDescriptor_total_bytes_offset_must_be_0x10[
    YGO_TYPE_OFFSET(FileTransferDescriptor, total_bytes) == 0x10 ? 1 : -1
];
typedef char FileTransferDescriptor_mode_offset_must_be_0x1C[
    YGO_TYPE_OFFSET(FileTransferDescriptor, mode) == 0x1C ? 1 : -1
];
typedef char FileTransferDescriptor_phase_callback_offset_must_be_0x20[
    YGO_TYPE_OFFSET(FileTransferDescriptor, phase_callback) == 0x20 ? 1 : -1
];
typedef char FileTransferDescriptor_absolute_lba_offset_must_be_0x24[
    YGO_TYPE_OFFSET(FileTransferDescriptor, absolute_lba) == 0x24 ? 1 : -1
];
typedef char FileTransferDescriptor_status_flags_offset_must_be_0x2C[
    YGO_TYPE_OFFSET(FileTransferDescriptor, status_flags) == 0x2C ? 1 : -1
];
typedef char FileTransferDescriptor_counter_offset_must_be_0x30[
    YGO_TYPE_OFFSET(FileTransferDescriptor, counter) == 0x30 ? 1 : -1
];
typedef char FileTransferDescriptor_field_32_offset_must_be_0x32[
    YGO_TYPE_OFFSET(FileTransferDescriptor, field_32) == 0x32 ? 1 : -1
];
typedef char FileTransferDescriptor_callback_data_offset_must_be_0x38[
    YGO_TYPE_OFFSET(FileTransferDescriptor, callback_data) == 0x38 ? 1 : -1
];
typedef char FileTransferDescriptor_done_offset_must_be_0x46[
    YGO_TYPE_OFFSET(FileTransferDescriptor, done) == 0x46 ? 1 : -1
];
typedef char FileTransferDescriptor_substate_offset_must_be_0x47[
    YGO_TYPE_OFFSET(FileTransferDescriptor, substate) == 0x47 ? 1 : -1
];
#undef YGO_TYPE_OFFSET

#define FADE_BAND_COUNT 30
#define FADE_TRANSITION_STATE_SIZE 0x28

typedef struct {
    u8 tint_r;
    u8 tint_g;
    u8 tint_b;
    u8 pad_03;
    u8 level;
    u8 target_level;
    u8 flags;
    u8 step;
    u16 field_08;
    u8 band_levels[FADE_BAND_COUNT];
} FadeTransitionState;

typedef char FadeTransitionState_size_must_be_0x28[
    sizeof(FadeTransitionState) == FADE_TRANSITION_STATE_SIZE ? 1 : -1
];
typedef char FadeTransitionState_level_offset_must_be_0x04[
    (u32)&((FadeTransitionState *)0)->level == 0x04 ? 1 : -1
];
typedef char FadeTransitionState_target_level_offset_must_be_0x05[
    (u32)&((FadeTransitionState *)0)->target_level == 0x05 ? 1 : -1
];
typedef char FadeTransitionState_flags_offset_must_be_0x06[
    (u32)&((FadeTransitionState *)0)->flags == 0x06 ? 1 : -1
];
typedef char FadeTransitionState_step_offset_must_be_0x07[
    (u32)&((FadeTransitionState *)0)->step == 0x07 ? 1 : -1
];
typedef char FadeTransitionState_field_08_offset_must_be_0x08[
    (u32)&((FadeTransitionState *)0)->field_08 == 0x08 ? 1 : -1
];
typedef char FadeTransitionState_band_levels_offset_must_be_0x0A[
    (u32)&((FadeTransitionState *)0)->band_levels == 0x0A ? 1 : -1
];

#endif
