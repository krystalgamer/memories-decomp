#ifndef YUGIOH_GAME_SOUND_H
#define YUGIOH_GAME_SOUND_H

#include "../types.h"

#define SD_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))
#define SD_COMMAND_QUEUE_COUNT 16
#define SD_COMMAND_RECORD_SIZE 0x30
#define SD_SECONDARY_OBJECT_COUNT 20
#define SD_SECONDARY_OBJECT_SIZE 0x28
#define SD_SEQUENCE_TRACK_COUNT 16
#define SD_SEQUENCE_TRACK_RECORD_SIZE 0x2C
#define SD_CHANNEL_VOLUME_MAX 0x80
#define SD_VOICE_SLOT_COUNT 4
#define SD_VOICE_SLOT_MASK_BASE 0x00100000
#define SD_VOICE_SLOT_MASK_ALL \
    ((SD_VOICE_SLOT_MASK_BASE << SD_VOICE_SLOT_COUNT) - \
     SD_VOICE_SLOT_MASK_BASE)

typedef struct {
    u8 command;
    u8 pad01[0x0F];
    s32 field_0010;
    s32 field_0014;
    s32 field_0018;
    s32 field_001C;
    s32 field_0020;
    s32 field_0024;
    s32 field_0028;
    s32 field_002C;
} SDCommand;

typedef struct {
    u8 pad00[4];
    u32 field_0004;
} SDValueLink;

typedef struct {
    u16 field_0000;
    u8 pad0002[2];
    u16 field_0004;
    u8 pad0006[0x36];
    u32 field_003C;
    u16 flags_0040;
    u16 mix_scale;
    u8 pad0044[4];
    u8 output_type;
    u8 field_0049;
    u8 flags_004A;
    u8 pad004B;
    s16 command_count;
    u8 pad004E[0x32];
    SDCommand commands[SD_COMMAND_QUEUE_COUNT];
    u8 pad0380[0x44];
    s32 field_03C4;
    s32 field_03C8;
    u16 field_03CC;
    u16 field_03CE;
    u8 pad03D0[0x34];
    u16 voice_ids[SD_VOICE_SLOT_COUNT];
    u8 pad040C[4];
    u8 voice_flags[SD_VOICE_SLOT_COUNT];
    u8 pad0414[0x10];
    u8 voice_value[SD_VOICE_SLOT_COUNT];
    u8 voice_step[SD_VOICE_SLOT_COUNT];
    u16 voice_timer[SD_VOICE_SLOT_COUNT];
    u8 voice_active_mask;
    u8 field_0435;
    u8 pad0436[2];
    u32 field_0438;
    u16 *field_043C;
    u16 field_0440;
    u16 field_0442;
    u8 pad0444[4];
    SDValueLink *field_0448;
    u8 pad044C[0xC4];
    s16 cd_volume;
    s16 field_0512;
    u8 channel_volume[2];
    u8 pad0516[0x1D];
    u8 mix_multiplier;
    u8 pad0534[8];
    u8 buffer_053C[4][0x200];
    u8 pad0D3C[0x800];
    u8 *buffer_ptrs_153C[4];
    u8 pad154C[0x14];
    u8 *field_1560;
    u16 *music_track;
    u8 pad1568[0x10];
    s16 field_1578;
    s16 field_157A;
    u8 pad157C[2];
    s16 field_157E;
    u8 pad1580[2];
    s16 field_1582;
    u8 field_1584;
    u8 pad1585;
    s16 field_1586;
    s16 field_1588;
    u8 pad158A[0x4E];
    u8 field_15D8[0x15];
    u8 field_15ED;
    u8 field_15EE;
    u8 field_15EF;
    u8 pad15F0[0x28];
    u8 busy;
    u8 pad1619[0x32];
    u8 field_164B;
} SDValue;

typedef struct {
    u8 field_0000;
    u8 field_0001;
    u8 pad0002;
    u8 field_0003;
    u8 pad0004;
    u8 field_0005;
    u8 field_0006;
    u8 field_0007;
    u8 pad0008[8];
    u8 field_0010;
    u8 field_0011;
    u8 field_0012;
    u8 field_0013;
    u8 pad0014[4];
} SDSecondaryRecord;

typedef struct {
    u8 pad0000[3];
    u8 field_0003;
    u8 pad0004[0x0B];
    u8 field_000F;
    u8 pad0010[0x0E];
    u16 field_001E;
    u8 pad0020[8];
} SDSecondaryObject;

typedef struct {
    s16 field_0000;
    u8 pad0002[2];
    u8 *field_0004;
    s32 field_0008;
    s32 field_000C;
    s32 field_0010;
    u8 *field_0014;
    u8 field_0018;
    u8 field_0019;
    u8 field_001A;
    u8 field_001B;
} SDSecondaryTransfer;

typedef struct {
    u8 pad0000[0x180];
    SDSecondaryObject objects[SD_SECONDARY_OBJECT_COUNT];
    u8 pad04A0[4];
    SDSecondaryTransfer transfer;
    u8 pad04C0[0x40];
    u8 flag_0500;
    u8 flag_0501;
    u8 flag_0502;
    u8 event_guard;
    long event_handle;
    u8 field_0508;
    u8 field_0509;
    u8 pad050A[2];
    void (*field_050C)(void);
    s16 object_count;
    s16 field_0512;
    s16 field_0514;
    s16 field_0516;
    u8 pad0518[0x2C4];
    u8 *field_07DC;
    s16 field_07E0;
    s16 field_07E2;
    s16 field_07E4;
    s16 field_07E6;
    u8 *field_07E8;
    s32 field_07EC;
    u8 pad07F0[0x0A];
    u16 field_07FA;
    u16 timebase;
    u8 pad07FE[2];
    u8 field_0800;
    u8 pad0801[3];
    s32 field_0804;
    s32 field_0808;
    s32 field_080C;
    s32 field_0810;
    u8 field_0814;
    u8 field_0815;
    u8 pad0816[2];
    u32 bytes_consumed;
    s32 field_081C;
    u8 pad0820[0x24];
    u8 field_0844;
    u8 field_0845;
    u8 pad0846[2];
} SDSecondaryState;

typedef char SDCommand_size_must_be_0x30[
    sizeof(SDCommand) == SD_COMMAND_RECORD_SIZE ? 1 : -1
];
typedef char SDValueLink_size_must_be_0x08[
    sizeof(SDValueLink) == 0x08 ? 1 : -1
];
typedef char SDValue_size_must_be_0x164C[
    sizeof(SDValue) == 0x164C ? 1 : -1
];
typedef char SDSecondaryObject_size_must_be_0x28[
    sizeof(SDSecondaryObject) == SD_SECONDARY_OBJECT_SIZE ? 1 : -1
];
typedef char SDSecondaryRecord_size_must_be_0x18[
    sizeof(SDSecondaryRecord) == 0x18 ? 1 : -1
];
typedef char SDSecondaryTransfer_size_must_be_0x1C[
    sizeof(SDSecondaryTransfer) == 0x1C ? 1 : -1
];
typedef char SDSecondaryState_size_must_be_0x848[
    sizeof(SDSecondaryState) == 0x848 ? 1 : -1
];
typedef char SDSecondaryState_objects_offset_must_be_0x180[
    SD_STATE_OFFSET(SDSecondaryState, objects) == 0x180 ? 1 : -1
];
typedef char SDSecondaryState_transfer_offset_must_be_0x4A4[
    SD_STATE_OFFSET(SDSecondaryState, transfer) == 0x4A4 ? 1 : -1
];
typedef char SDSecondaryState_flag_0500_offset_must_be_0x500[
    SD_STATE_OFFSET(SDSecondaryState, flag_0500) == 0x500 ? 1 : -1
];
typedef char SDSecondaryState_event_guard_offset_must_be_0x503[
    SD_STATE_OFFSET(SDSecondaryState, event_guard) == 0x503 ? 1 : -1
];
typedef char SDSecondaryState_event_handle_offset_must_be_0x504[
    SD_STATE_OFFSET(SDSecondaryState, event_handle) == 0x504 ? 1 : -1
];
typedef char SDSecondaryState_object_count_offset_must_be_0x510[
    SD_STATE_OFFSET(SDSecondaryState, object_count) == 0x510 ? 1 : -1
];
typedef char SDSecondaryState_field_07DC_offset_must_be_0x7DC[
    SD_STATE_OFFSET(SDSecondaryState, field_07DC) == 0x7DC ? 1 : -1
];
typedef char SDSecondaryState_field_07E0_offset_must_be_0x7E0[
    SD_STATE_OFFSET(SDSecondaryState, field_07E0) == 0x7E0 ? 1 : -1
];
typedef char SDSecondaryState_field_07FA_offset_must_be_0x7FA[
    SD_STATE_OFFSET(SDSecondaryState, field_07FA) == 0x7FA ? 1 : -1
];
typedef char SDSecondaryState_timebase_offset_must_be_0x7FC[
    SD_STATE_OFFSET(SDSecondaryState, timebase) == 0x7FC ? 1 : -1
];
typedef char SDSecondaryState_bytes_consumed_offset_must_be_0x818[
    SD_STATE_OFFSET(SDSecondaryState, bytes_consumed) == 0x818 ? 1 : -1
];
typedef char SDSecondaryState_field_081C_offset_must_be_0x81C[
    SD_STATE_OFFSET(SDSecondaryState, field_081C) == 0x81C ? 1 : -1
];
typedef char SDSecondaryState_field_0844_offset_must_be_0x844[
    SD_STATE_OFFSET(SDSecondaryState, field_0844) == 0x844 ? 1 : -1
];

#undef SD_STATE_OFFSET

#ifndef SDVALUE_CUSTOM_EXTERN
extern SDValue *g_SDValue;
#endif

#ifndef SDSECONDARYSTATE_CUSTOM_EXTERN
extern SDSecondaryState *D_8009B458;
#endif

#endif
