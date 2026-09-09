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

typedef u8 *(*ModelHandler)(u8 **);
typedef void (*ScriptCommandHandler)(void);

typedef struct {
    u8 b[11];
} SDInitBlk11;

typedef struct {
    u8 b[10];
} SDInitBlk10;

typedef struct {
    s32 key;
    void **handler;
} ModelHandlerObject;

#define YGO_TYPE_OFFSET(type, member) ((u32)&(((type *)0)->member))

/* The two endpoint blocks in each model-effect request. `packed` preserves
   func_8005F91C's unaligned two-word copies from generic byte pointers. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 kind;
} __attribute__((packed)) ModelEffectEndpoint;

typedef char ModelEffectEndpoint_size_must_be_0x8[
    sizeof(ModelEffectEndpoint) == 0x8 ? 1 : -1
];
typedef char ModelEffectEndpoint_kind_offset_must_be_0x6[
    YGO_TYPE_OFFSET(ModelEffectEndpoint, kind) == 0x6 ? 1 : -1
];

/* Four halfwords produced by func_80059000. The fourth is the maximum of the
   first three after the slot-specific overrides are applied. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 max;
} ModelEffectAdjustment;

typedef char ModelEffectAdjustment_size_must_be_0x8[
    sizeof(ModelEffectAdjustment) == 0x8 ? 1 : -1
];
typedef char ModelEffectAdjustment_max_offset_must_be_0x6[
    YGO_TYPE_OFFSET(ModelEffectAdjustment, max) == 0x6 ? 1 : -1
];

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
    /* One word at 0x30 that the loader's phase callbacks write both ways,
       so the record carries both readings rather than picking one.

       As two halfwords it is a pair: duel_card_data_transfer.c counts
       sectors in `counter` alone, the phase-2 paths write a VRAM position
       pair, and file_stream.c splits a byte count across the two halves.
       As one word it is a single packed constant, which is what the phase-3
       paths of func_8002BD0C and func_800577B0 store in one instruction.

       Neither view is a superset of the other and retail emits both, so
       collapsing them to either alone changes codegen: spelling the phase-3
       word as two halfword stores costs an instruction at each site and
       overflows .text. */
    union {
        struct {
            u16 counter;
            u16 field_32;
        } h;
        u32 word;
    } field_30;
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
    YGO_TYPE_OFFSET(FileTransferDescriptor, field_30.h.counter) == 0x30 ? 1 : -1
];
typedef char FileTransferDescriptor_field_32_offset_must_be_0x32[
    YGO_TYPE_OFFSET(FileTransferDescriptor, field_30.h.field_32) == 0x32 ? 1 : -1
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

/* Display-object script state shared by the five handlers at 0x8004141C.
   The two pointers delimit the script buffer and the trailing halfwords are
   handler status fields. */
typedef struct {
    u8 pad_00[4];
    u32 flags;
    u8 pad_08[0x48];
    u8 *current;
    u8 *base;
    s16 field_58;
    s16 field_5A;
} DisplayObjectStreamState;

typedef char DisplayObjectStreamState_size_must_be_0x5C[
    sizeof(DisplayObjectStreamState) == 0x5C ? 1 : -1
];
typedef char DisplayObjectStreamState_current_offset_must_be_0x50[
    (u32)&((DisplayObjectStreamState *)0)->current == 0x50 ? 1 : -1
];
typedef char DisplayObjectStreamState_base_offset_must_be_0x54[
    (u32)&((DisplayObjectStreamState *)0)->base == 0x54 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_58_offset_must_be_0x58[
    (u32)&((DisplayObjectStreamState *)0)->field_58 == 0x58 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_5A_offset_must_be_0x5A[
    (u32)&((DisplayObjectStreamState *)0)->field_5A == 0x5A ? 1 : -1
];

typedef struct {
    u8 pad_00[0x14];
    u16 priority;
    u8 pad_16;
    u8 ot_index;
    u8 pad_18[0x18];
    u16 field_30;
    u16 field_32;
} DuelStatusPosition;

typedef struct {
    u8 pad_00[0x50];
    DuelStatusPosition *field_50;
} DuelStatusWidget;

typedef struct {
    u32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u16 field_0C;
    u16 field_0E;
    u32 field_10;
    u32 field_14;
} DuelStatusDigitPacket;

typedef char DuelStatusPosition_size_must_be_0x34[
    sizeof(DuelStatusPosition) == 0x34 ? 1 : -1
];
typedef char DuelStatusPosition_priority_offset_must_be_0x14[
    (u32)&((DuelStatusPosition *)0)->priority == 0x14 ? 1 : -1
];
typedef char DuelStatusPosition_ot_index_offset_must_be_0x17[
    (u32)&((DuelStatusPosition *)0)->ot_index == 0x17 ? 1 : -1
];
typedef char DuelStatusPosition_field_30_offset_must_be_0x30[
    (u32)&((DuelStatusPosition *)0)->field_30 == 0x30 ? 1 : -1
];
typedef char DuelStatusWidget_field_50_offset_must_be_0x50[
    (u32)&((DuelStatusWidget *)0)->field_50 == 0x50 ? 1 : -1
];
typedef char DuelStatusDigitPacket_size_must_be_0x18[
    sizeof(DuelStatusDigitPacket) == 0x18 ? 1 : -1
];
typedef char DuelStatusDigitPacket_field_14_offset_must_be_0x14[
    (u32)&((DuelStatusDigitPacket *)0)->field_14 == 0x14 ? 1 : -1
];

#endif
