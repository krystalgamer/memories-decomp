#ifndef MEMORIES_DECOMP_YGO_TYPES_H
#define MEMORIES_DECOMP_YGO_TYPES_H

#include "types.h"

/* Only the first five words at 0x80010000: three reused MODEL payload
 * bases, followed by the two primary modules entered at +4. This is an
 * address-block prefix, not a homogeneous arena table or a payload layout.
 * The remaining module, data-argument and SU words keep separate labels. */
typedef struct {
    u8 *payload_bases[3];
    u8 *primary_modules[2];
} HighMemoryModelAddressPrefix;

typedef char HighMemoryModelAddressPrefix_size_must_be_0x14[
    sizeof(HighMemoryModelAddressPrefix) == 0x14 ? 1 : -1
];
typedef char HighMemoryModelAddressPrefix_modules_offset_must_be_0xC[
    (u32)&(((HighMemoryModelAddressPrefix *)0)->primary_modules) == 0xC ? 1 : -1
];

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
    u8 bytes[20];
} TextDecimalDigitKeyBlock;

typedef char TextDecimalDigitKeyBlock_size_must_be_20[
    sizeof(TextDecimalDigitKeyBlock) == 20 ? 1 : -1
];

typedef struct {
    u16 positions[3];
} OptionsLayoutPositionBlock;

typedef char OptionsLayoutPositionBlock_size_must_be_6[
    sizeof(OptionsLayoutPositionBlock) == 6 ? 1 : -1
];

typedef struct {
    OptionsLayoutPositionBlock positions;
    u16 pad_06;
} OptionsLayoutPositionData;

typedef char OptionsLayoutPositionData_size_must_be_8[
    sizeof(OptionsLayoutPositionData) == 8 ? 1 : -1
];

typedef struct {
    s32 key;
    void **handler;
} ModelHandlerObject;

#define YGO_TYPE_OFFSET(type, member) ((u32)&(((type *)0)->member))

/* Live overworld records: camera setup/tween, marker placement and exit
   selection all read the same 66-byte stride. The alternate map is separate. */
typedef struct {
    u16 story_flag;
    s16 x;
    s16 y;
    u16 input_mask;
    u8 field_08;
    u8 destination;
    u8 move_steps;
    u8 pad_0B;
} CampaignMapExit;

typedef struct {
    u16 confirm_gate;
    s16 camera_field_04;
    s16 camera_angle;
    s16 camera_field_00;
    s16 view_x;
    s16 view_z;
    s16 f12;
    s16 f14;
    u8 confirm_destination;
    u8 pad_11;
    CampaignMapExit exits[4];
} MapLocation;

typedef char CampaignMapExit_size_must_be_12[
    sizeof(CampaignMapExit) == 12 ? 1 : -1
];
typedef char CampaignMapExit_input_mask_offset_must_be_6[
    YGO_TYPE_OFFSET(CampaignMapExit, input_mask) == 6 ? 1 : -1
];
typedef char CampaignMapExit_destination_offset_must_be_9[
    YGO_TYPE_OFFSET(CampaignMapExit, destination) == 9 ? 1 : -1
];
typedef char MapLocation_size_must_be_66[
    sizeof(MapLocation) == 66 ? 1 : -1
];
typedef char MapLocation_marker_offset_must_be_12[
    YGO_TYPE_OFFSET(MapLocation, f12) == 12 ? 1 : -1
];
typedef char MapLocation_exits_offset_must_be_18[
    YGO_TYPE_OFFSET(MapLocation, exits) == 18 ? 1 : -1
];

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[20];
    u16 f72;
    u16 f74;
    u8 pad76[20];
    s16 f96;
} MapObject;

typedef char MapObject_position_offset_must_be_48[
    YGO_TYPE_OFFSET(MapObject, f48) == 48 ? 1 : -1
];
typedef char MapObject_transition_offset_must_be_96[
    YGO_TYPE_OFFSET(MapObject, f96) == 96 ? 1 : -1
];

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
    u32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u16 field_0C;
    u16 field_0E;
    u32 field_10;
    u32 field_14;
} DuelStatusDigitPacket;

typedef char DuelStatusDigitPacket_size_must_be_0x18[
    sizeof(DuelStatusDigitPacket) == 0x18 ? 1 : -1
];
typedef char DuelStatusDigitPacket_field_14_offset_must_be_0x14[
    (u32)&((DuelStatusDigitPacket *)0)->field_14 == 0x14 ? 1 : -1
];

/* Two words written together as a pair. free_duel/screen_runtime.c and
 * password/shop.c formerly defined this identically to view D_801D5608, and
 * each writes both members at once -- the low word from a table entry and the
 * high word from the index beside it.
 *
 * The typedef is shared here; the extern that uses it is not. D_801D5608 is a
 * staging area with several faithful views, spelled s32 and s32 [] elsewhere
 * and reached through an explicit .reloc in main_run_credits.c, so each
 * consumer keeps the declaration its own reads need. See notes/build.md. */
typedef struct {
    u32 lo;
    u32 hi;
} Pair;

typedef char Pair_size_must_be_8[
    sizeof(Pair) == 8 ? 1 : -1
];

#endif
