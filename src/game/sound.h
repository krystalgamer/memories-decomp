#ifndef YUGIOH_GAME_SOUND_H
#define YUGIOH_GAME_SOUND_H

#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_sequence_constants.h"
#include "sound_voice_constants.h"

#define SD_STATE_OFFSET(type, member) ((u32)&(((type *)0)->member))
#define SD_COMMAND_QUEUE_COUNT 16
#define SD_COMMAND_RECORD_SIZE 0x30
#define SD_COMMAND_QUEUE_BYTE_OFFSET 0x80
#define SD_VALUE_LINK_RECORD_SIZE 0x08
#define SD_VALUE_LINK_INDEX_MASK 0xFFFF
#define SD_VALUE_LINK_INDEX_NONE 0xFFFF
#define SD_SECONDARY_LEVEL_MASK 0x7F
#define SD_SECONDARY_LEVEL_MAX SD_SECONDARY_LEVEL_MASK
#define SD_COMMAND_VALUE_MASK 0xFFFF
#define SD_BGM_COMMAND_BASE 0x7000
#define SD_SE_VOLUME_MAX 0xFF
#define SD_CHANNEL_VOLUME_MAX 0x80
#define SD_MIX_SAMPLE_COUNT 256
#define SD_KEY_OFF_RETRY_LIMIT 256
#define SD_TRANSFER_STATE_INACTIVE (-1)
#define SD_TRANSFER_ERROR (-1)
#define SD_TRANSFER_INCOMPLETE (-2)

typedef struct {
    u8 command;
    u8 field_0001;
    s16 field_0002;
    s32 field_0004;
    s32 field_0008;
    s32 field_000C;
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
    u32 words[8];
} SDCommandTail;

typedef struct {
    u8 pad00[4];
    u32 field_0004;
} SDValueLink;

typedef struct {
    u8 volume;
    u8 timer;
    u8 pad0002[2];
    u16 pitch;
    u16 field_0006;
} SDNote;

typedef struct {
    u16 field_0000;
    u16 field_0002;
    u16 field_0004;
    u8 pad0006[0x36];
    u32 field_003C;
    u16 flags_0040;
    u16 mix_scale;
    u16 field_0044;
    u8 pad0046[2];
    u8 output_type;
    u8 field_0049;
    u8 flags_004A;
    u8 pad004B;
    s16 command_count;
    u16 field_004E;
    u32 field_0050;
    u32 field_0054;
    u32 field_0058;
    u32 field_005C[8];
    u8 field_007C;
    u8 field_007D;
    u8 field_007E;
    u8 pad007F;
    SDCommand commands[SD_COMMAND_QUEUE_COUNT];
    u8 pad0380[4];
    /* The staged SpuVoiceAttr the driver keys voices on with. func_8004803C
       fills in the live half per sound effect -- `voice` as the key bitmask,
       `volume` from the note table and the pan, `note` as the pitch and `addr`
       as the waveform address -- and hands it to SpuSetKeyOnWithAttr. The
       constant half is written once by func_80047480: `mask` 0xFFFF, unity
       `pitch`, `sample_note` 0x3C00, the three envelope rate modes and zeroed
       ADSR. Both were reaching it by offset before; the region is exactly one
       SpuVoiceAttr wide, which the assertion below pins. */
    SpuVoiceAttr voice_attr;
    s32 field_03C4;
    s32 field_03C8;
    u16 field_03CC;
    u16 field_03CE;
    u8 pad03D0[0x34];
    u16 voice_ids[SD_VOICE_SLOT_COUNT];
    u8 field_040C[SD_VOICE_SLOT_COUNT];
    u8 voice_flags[SD_VOICE_SLOT_COUNT];
    u16 voice_volume_left[SD_VOICE_SLOT_COUNT];
    u16 voice_volume_right[SD_VOICE_SLOT_COUNT];
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
    SDNote *field_0444;
    SDValueLink *field_0448;
    u16 field_044C[SD_VOICE_LOOKUP_BANK_COUNT][SD_VOICE_LOOKUP_BANK_ENTRY_COUNT];
    u8 pad04CC[0x510 - SD_VOICE_LOOKUP_END_BYTE_OFFSET];
    s16 cd_volume;
    s16 field_0512;
    u8 channel_volume[2];
    u8 pad0516[2];
    /* The three mixer-out bank bases func_80046A08 installs once the
       "VolInf" signature checks out: the bank itself and the two records
       that follow it. */
    u8 *bank_0518[3];
    u8 pad0524[0xF];
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
    s16 field_1580;
    s16 field_1582;
    u8 field_1584;
    u8 pad1585;
    s16 field_1586;
    s16 field_1588;
    u8 field_158A;
    u8 pad158B[0x4D];
    u8 field_15D8[0x14];
    u8 field_15EC;
    u8 field_15ED;
    u8 field_15EE;
    u8 field_15EF;
    u8 pad15F0[4];
    s16 field_15F4;
    u8 pad15F6[0x22];
    u8 busy;
    u8 pad1619[0x30];
    /* The two "VolInf" trailer bytes, one per mixer-out bank; func_80046A08
       latches each into field_0042 / field_0044 as it loads them. */
    u8 field_1649;
    u8 field_164A;
    u8 field_164B;
} SDValue;

typedef struct {
    u8 program;
    u8 pan;
    u8 pad0002;
    u8 volume;
    u8 pad0004;
    u8 expression;
    u8 field_0006;
    u8 pitch_bend_msb;
    u8 pad0008[8];
    u8 field_0010;
    u8 parameter_selector;
    u8 control_mode;
    u8 control_value;
    u8 pad0014[4];
} SDSecondaryRecord;

typedef struct {
    u8 voice_index;
    u8 pad0001[2];
    u8 channel_index;
    u8 pad0004[2];
    u8 note;
    u8 pad0007;
    u8 field_0008;
    u8 field_0009;
    u8 field_000A;
    u8 field_000B;
    u8 field_000C;
    u8 pad000D;
    u8 field_000E;
    u8 field_000F;
    u8 pitch_bend_positive_scale;
    u8 pitch_bend_negative_scale;
    u8 field_0012;
    u8 field_0013;
    u16 field_0014;
    u16 field_0016;
    u8 pad0018[2];
    s16 cached_pitch_bend;
    u8 pad001C[2];
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

/* One MIDI track reader inside SDSecondaryState, at +0x518. `pos` is a byte
   offset into the sequence data at SDSecondaryState::field_07DC, which is how
   SD_ReadSequenceByte reads through it; the chunk triple is filled in by
   SD_OpenSequenceTrack from the MTrk header. Every `_saved` field is the copy taken
   by controller 0x63/0x14 (loop start) and put back by 0x63/0x1E (loop end)
   in SD_DispatchSequenceChannelEvent. */
typedef struct {
    s32 pos;
    s32 pos_saved;
    s32 chunk_length;
    s32 chunk_end;
    s32 chunk_start;
    u16 tempo_accumulator;
    u16 tempo_step;
    u16 field_0018;
    u16 field_0018_saved;
    u32 delta_remaining;
    u32 delta_remaining_saved;
    u8 ended;
    u8 ended_saved;
    u8 loop_count;
    u8 field_0027;
    u8 running_status_held;
    u8 running_status;
    u8 running_status_saved;
    u8 pad002B;
} SDSequenceTrack;

typedef struct {
    SDSecondaryRecord channels[SD_SEQUENCE_CHANNEL_COUNT];
    SDSecondaryObject objects[SD_SECONDARY_OBJECT_COUNT];
    u8 pad04A0[4];
    SDSecondaryTransfer transfer;
    SpuVoiceAttr voice_attr;
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
    SDSequenceTrack tracks[SD_SEQUENCE_TRACK_COUNT];
    u8 pad07D8[4];
    u8 *field_07DC;
    s16 field_07E0;
    s16 field_07E2;
    s16 field_07E4;
    s16 field_07E6;
    u8 *field_07E8;
    s32 field_07EC;
    s32 field_07F0;
    s32 field_07F4;
    u16 field_07F8;
    u16 track_count;
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
typedef char SDCommandTail_size_must_be_0x20[
    sizeof(SDCommandTail) == 0x20 ? 1 : -1
];
typedef char SDCommand_field_0002_offset_must_be_0x02[
    SD_STATE_OFFSET(SDCommand, field_0002) == 0x02 ? 1 : -1
];
typedef char SDCommand_field_0004_offset_must_be_0x04[
    SD_STATE_OFFSET(SDCommand, field_0004) == 0x04 ? 1 : -1
];
typedef char SDCommand_field_0010_offset_must_be_0x10[
    SD_STATE_OFFSET(SDCommand, field_0010) == 0x10 ? 1 : -1
];
typedef char SDNote_size_must_be_0x08[
    sizeof(SDNote) == SD_NOTE_RECORD_SIZE ? 1 : -1
];
typedef char SDValueLink_size_must_be_0x08[
    sizeof(SDValueLink) == SD_VALUE_LINK_RECORD_SIZE ? 1 : -1
];
typedef char SDValue_size_must_be_0x164C[
    sizeof(SDValue) == 0x164C ? 1 : -1
];
typedef char SDValue_lookup_bank_size_must_match_stride[
    sizeof(((SDValue *)0)->field_044C[0]) ==
        SD_VOICE_LOOKUP_BANK_BYTE_STRIDE ? 1 : -1
];
typedef char SDVoiceLookup_tag_must_fit_code_mask[
    (SD_VOICE_LOOKUP_CODE_TAG & SD_VOICE_LOOKUP_CODE_MASK) ==
        SD_VOICE_LOOKUP_CODE_TAG ? 1 : -1
];
typedef char SDVoiceLookup_selector_fields_must_not_overlap[
    (SD_VOICE_LOOKUP_CODE_MASK &
        (SD_VOICE_LOOKUP_INDEX_MASK | SD_VOICE_LOOKUP_BANK_FLAG)) == 0 &&
    (SD_VOICE_LOOKUP_INDEX_MASK & SD_VOICE_LOOKUP_BANK_FLAG) == 0 ? 1 : -1
];
typedef char SDValue_lookup_offset_must_be_0x44C[
    SD_STATE_OFFSET(SDValue, field_044C) ==
        SD_VOICE_LOOKUP_BYTE_OFFSET ? 1 : -1
];
typedef char SDValue_lookup_end_must_match_extent[
    SD_STATE_OFFSET(SDValue, field_044C) +
        sizeof(((SDValue *)0)->field_044C) ==
        SD_VOICE_LOOKUP_END_BYTE_OFFSET ? 1 : -1
];
typedef char SDValue_commands_offset_must_be_0x80[
    SD_STATE_OFFSET(SDValue, commands) == SD_COMMAND_QUEUE_BYTE_OFFSET ? 1 : -1
];
typedef char SDValue_voice_attr_offset_must_be_0x384[
    SD_STATE_OFFSET(SDValue, voice_attr) == 0x384 ? 1 : -1
];
typedef char SDValue_voice_attr_note_offset_must_be_0x39A[
    SD_STATE_OFFSET(SDValue, voice_attr) +
        SD_STATE_OFFSET(SpuVoiceAttr, note) == 0x39A ? 1 : -1
];
typedef char SDValue_voice_attr_must_end_at_field_03C4[
    SD_STATE_OFFSET(SDValue, voice_attr) + sizeof(SpuVoiceAttr) ==
        SD_STATE_OFFSET(SDValue, field_03C4) ? 1 : -1
];
typedef char SDValue_field_0044_offset_must_be_0x44[
    SD_STATE_OFFSET(SDValue, field_0044) == 0x44 ? 1 : -1
];
typedef char SDValue_field_0002_offset_must_be_0x02[
    SD_STATE_OFFSET(SDValue, field_0002) == 0x02 ? 1 : -1
];
typedef char SDValue_bank_0518_offset_must_be_0x518[
    SD_STATE_OFFSET(SDValue, bank_0518) == 0x518 ? 1 : -1
];
typedef char SDValue_mix_multiplier_offset_must_be_0x533[
    SD_STATE_OFFSET(SDValue, mix_multiplier) == 0x533 ? 1 : -1
];
typedef char SDValue_field_1649_offset_must_be_0x1649[
    SD_STATE_OFFSET(SDValue, field_1649) == 0x1649 ? 1 : -1
];
typedef char SDValue_field_164A_offset_must_be_0x164A[
    SD_STATE_OFFSET(SDValue, field_164A) == 0x164A ? 1 : -1
];
typedef char SDValue_field_004E_offset_must_be_0x4E[
    SD_STATE_OFFSET(SDValue, field_004E) == 0x4E ? 1 : -1
];
typedef char SDValue_field_007C_offset_must_be_0x7C[
    SD_STATE_OFFSET(SDValue, field_007C) == 0x7C ? 1 : -1
];
typedef char SDValue_field_1580_offset_must_be_0x1580[
    SD_STATE_OFFSET(SDValue, field_1580) == 0x1580 ? 1 : -1
];
typedef char SDValue_field_158A_offset_must_be_0x158A[
    SD_STATE_OFFSET(SDValue, field_158A) == 0x158A ? 1 : -1
];
typedef char SDValue_field_15EC_offset_must_be_0x15EC[
    SD_STATE_OFFSET(SDValue, field_15EC) == 0x15EC ? 1 : -1
];
typedef char SDValue_field_15F4_offset_must_be_0x15F4[
    SD_STATE_OFFSET(SDValue, field_15F4) == 0x15F4 ? 1 : -1
];
typedef char SDSecondaryObject_size_must_be_0x28[
    sizeof(SDSecondaryObject) == SD_SECONDARY_OBJECT_SIZE ? 1 : -1
];
typedef char SDSecondaryObject_channel_index_offset_must_be_0x03[
    SD_STATE_OFFSET(SDSecondaryObject, channel_index) == 0x03 ? 1 : -1
];
typedef char SDSecondaryRecord_size_must_be_0x18[
    sizeof(SDSecondaryRecord) == SD_SEQUENCE_CHANNEL_RECORD_SIZE ? 1 : -1
];
typedef char SDSecondaryRecord_program_offset_must_be_0x00[
    SD_STATE_OFFSET(SDSecondaryRecord, program) == 0x00 ? 1 : -1
];
typedef char SDSecondaryRecord_pan_offset_must_be_0x01[
    SD_STATE_OFFSET(SDSecondaryRecord, pan) == 0x01 ? 1 : -1
];
typedef char SDSecondaryRecord_volume_offset_must_be_0x03[
    SD_STATE_OFFSET(SDSecondaryRecord, volume) == 0x03 ? 1 : -1
];
typedef char SDSecondaryRecord_expression_offset_must_be_0x05[
    SD_STATE_OFFSET(SDSecondaryRecord, expression) == 0x05 ? 1 : -1
];
typedef char SDSecondaryRecord_pitch_bend_msb_offset_must_be_0x07[
    SD_STATE_OFFSET(SDSecondaryRecord, pitch_bend_msb) == 0x07 ? 1 : -1
];
typedef char SDSecondaryRecord_parameter_selector_offset_must_be_0x11[
    SD_STATE_OFFSET(SDSecondaryRecord, parameter_selector) == 0x11 ? 1 : -1
];
typedef char SDSecondaryRecord_control_mode_offset_must_be_0x12[
    SD_STATE_OFFSET(SDSecondaryRecord, control_mode) == 0x12 ? 1 : -1
];
typedef char SDSecondaryRecord_control_value_offset_must_be_0x13[
    SD_STATE_OFFSET(SDSecondaryRecord, control_value) == 0x13 ? 1 : -1
];
typedef char SDSequenceTrack_size_must_be_0x2C[
    sizeof(SDSequenceTrack) == SD_SEQUENCE_TRACK_RECORD_SIZE ? 1 : -1
];
typedef char SDSequenceTrack_tempo_accumulator_offset_must_be_0x14[
    SD_STATE_OFFSET(SDSequenceTrack, tempo_accumulator) == 0x14 ? 1 : -1
];
typedef char SDSequenceTrack_delta_remaining_offset_must_be_0x1C[
    SD_STATE_OFFSET(SDSequenceTrack, delta_remaining) == 0x1C ? 1 : -1
];
typedef char SDSequenceTrack_ended_offset_must_be_0x24[
    SD_STATE_OFFSET(SDSequenceTrack, ended) == 0x24 ? 1 : -1
];
typedef char SDSequenceTrack_running_status_offset_must_be_0x29[
    SD_STATE_OFFSET(SDSequenceTrack, running_status) == 0x29 ? 1 : -1
];
typedef char SDSecondaryTransfer_size_must_be_0x1C[
    sizeof(SDSecondaryTransfer) == 0x1C ? 1 : -1
];
typedef char SDSecondaryState_size_must_be_0x848[
    sizeof(SDSecondaryState) == 0x848 ? 1 : -1
];
typedef char SDSecondaryState_channels_offset_must_be_0x00[
    SD_STATE_OFFSET(SDSecondaryState, channels) == 0x00 ? 1 : -1
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
typedef char SDSecondaryState_voice_attr_offset_must_be_0x4C0[
    SD_STATE_OFFSET(SDSecondaryState, voice_attr) == 0x4C0 ? 1 : -1
];
typedef char SpuVoiceAttr_size_must_be_0x40[
    sizeof(SpuVoiceAttr) == 0x40 ? 1 : -1
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
typedef char SDSecondaryState_tracks_offset_must_be_0x518[
    SD_STATE_OFFSET(SDSecondaryState, tracks) ==
        SD_SEQUENCE_TRACK_ARRAY_OFFSET ? 1 : -1
];
typedef char SDSecondaryState_track_count_offset_must_be_0x7FA[
    SD_STATE_OFFSET(SDSecondaryState, track_count) == 0x7FA ? 1 : -1
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
/* Two translation units need a different spelling of this one declaration,
 * and both are codegen inputs rather than style:
 *
 *   G_SDVALUE_AGGREGATE -- an unsized array extern is not small data, so
 *   cc1psx emits the lui %hi / lw %lo pair instead of one gp-relative load.
 *   func_800464F0 needs the two-instruction form.
 *
 *   G_SDVALUE_VOLATILE -- func_80049138 reads the pointer three times and
 *   retail reloads it each time; without the qualifier gcc commons the
 *   first read and the reloads disappear.
 *
 * Everything else takes the plain declaration. */
#ifdef G_SDVALUE_AGGREGATE
extern SDValue *g_SDValue[];
#elif defined(G_SDVALUE_VOLATILE)
extern SDValue *volatile g_SDValue;
#else
extern SDValue *g_SDValue;
#endif
#endif

#ifndef SDSECONDARYSTATE_CUSTOM_EXTERN
extern SDSecondaryState *D_8009B458;
#endif

/* One SPU voice bit per entry.  The object at D_80011434 is twenty words
 * holding 1 << n for n = 0 .. 19, read out of the retail image.  The uses
 * agree that these are voice masks: func_8004A27C submits D_80011434[voice]
 * as the `voice` field of the SpuVoiceAttr it hands to SpuSetVoiceAttr, and
 * func_8004A7C0 passes an entry straight to SpuSetKey and SpuGetKeyStatus,
 * both of which take a voice mask.
 *
 * D_80011434_IS_CONST is a codegen input, measured rather than assumed:
 * with sound_voice_envelope.c on the plain declaration that unit compiles to
 * 204 bytes of text instead of 200 and the executable stops linking, because
 * .initialized_data then overlaps .text.  Nothing else needs the qualifier.
 */
#ifdef D_80011434_IS_CONST
extern const s32 D_80011434[20];
#else
extern s32 D_80011434[20];
#endif

void Sound_InitFrontend(void);
void SD_InitState(u8);
s32 SD_EnqueueCommand(SDCommand *);
void SD_UpdateFades(void);
void SD_UpdateRuntime(void);
void SD_BGMPlay(u32);
void SD_SEPlayFull(u32);
void SD_BGMFadeOut(void);
void SD_BGMFadeOutWithStep(s32);
void SD_SetOutputType(s16);
void SD_KeyOffVoiceSlots(void);
void SD_StopAll(void);

/* gSD_dwCurrentBgmCommand is deliberately not declared here.
 *
 * sound_frontend.c writes it as a scalar, which -G8 reaches gp-relative,
 * while script_stream_commands.c and duel_effect_play_sound_command.c read it
 * through `extern u32 gSD_dwCurrentBgmCommand[]` and `[0]`: an unsized array
 * is not assumed small, so those reads are built from an absolute address.
 * Giving all three the scalar form builds the executable eight bytes short, so
 * the split is load-bearing. Declaring it here would force one spelling on
 * every includer, which is what kept those two files out of this header.
 */

#endif
