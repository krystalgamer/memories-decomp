#ifndef YUGIOH_GAME_MODEL_H
#define YUGIOH_GAME_MODEL_H

#include "../types.h"

#define MODEL_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define MODEL_SLOT_COUNT 3
#define MODEL_SLOT_SIZE 0xE20
#define MODEL_SLOT_DATA_ENTRY_SIZE 80
#define MODEL_HANDLER_REGISTRY_COUNT 80
#define MODEL_TINT_REQUEST_COUNT 10
#define MODEL_SLOT_SOUND_ENTRY_COUNT 64
#define MODEL_SLOT_PART_COUNT 58
#define MODEL_SLOT_UNIT_COUNT 60
#define MODEL_SLOT_ROW_COUNT 10
#define MODEL_SLOT_ROW_KEY_TABLE_OFFSET 0x2C8
#define MODEL_SLOT_CF8_DFE_OFFSET 0x106
#define MODEL_SLOT_CF8_DFF_OFFSET 0x107
#define MODEL_DATA_MIN_FREE_BYTES 0x401
#define MODEL_LIGHT_BASE_INTENSITY 128
#define MODEL_LIGHT_DIM_INTENSITY (MODEL_LIGHT_BASE_INTENSITY / 2)
#define MODEL_FIXED_ONE 0x1000
#define MODEL_FIXED_HALF (MODEL_FIXED_ONE / 2)
#define MODEL_FIXED_NEGATIVE_ONE (-MODEL_FIXED_ONE)
#define MODEL_FIXED_THREE (MODEL_FIXED_ONE * 3)
#define MODEL_ANGLE_FULL_TURN 0x1000
#define MODEL_ANGLE_QUARTER_TURN (MODEL_ANGLE_FULL_TURN / 4)
#define MODEL_ANGLE_HALF_TURN (MODEL_ANGLE_FULL_TURN / 2)
#define MODEL_ANGLE_WRAP_THRESHOLD (MODEL_ANGLE_HALF_TURN + 1)
#define MODEL_ANGLE_MASK (MODEL_ANGLE_FULL_TURN - 1)
#define MODEL_DEFAULT_PROJECTION 0x12C
#define MODEL_SPECIAL_BATTLE_ID 0x309
#define MODEL_MRG_ID_END 0x2D2
#define MODEL_MRG_LAST_ID (MODEL_MRG_ID_END - 1)
#define MODEL_MRG_FIRST_GAP_START 0x12C
#define MODEL_MRG_FIRST_GAP_END 0x15E
#define MODEL_MRG_SECOND_GAP_START 0x28A
#define MODEL_MRG_SECOND_GAP_END 0x2BC
#define MODEL_MRG_SINGLE_GAP_ID 0x2D0
#define MODEL_MRG_GAP_SIZE \
    (MODEL_MRG_FIRST_GAP_END - MODEL_MRG_FIRST_GAP_START)
#define MODEL_MRG_SECTOR_COUNT 0x114
/* The European build's wrapper defines its own. */
#ifndef MODEL_SPECIAL_BATTLE_FILE_START_SECTOR
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x3B4
#endif
#define MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT 0x113
#define MODEL_AUX_SECTOR_COUNT 0x74
/* The European build's wrapper defines its own. */
#ifndef MODEL_AUX_FILE_START_SECTOR
#define MODEL_AUX_FILE_START_SECTOR 0x88
#endif
#define MODEL_AUX_LOOKUP_RECORD_SIZE 0xB2
#define MODEL_AUX_LOOKUP_VALUE_OFFSET 0xA0

/* GsUNIT view retaining the original integer coordinate-address storage. */
typedef struct {
    u32 coordinate_address;
    void *G32 primitive_data;
} ModelSlotHeadEntry;

/* One of the MODEL_SLOT_ROW_COUNT accumulator rows at slot offset 0x750.
 * Model_ResetSlotAnimationTables walks the rows and their maxima in the same
 * loop
 * iteration -- 58 halfwords at 0x750 + row * 0x76 and the halfword at
 * 0x7C4 + row * 0x76 -- which is what groups `values` and `max` into one
 * 0x76-byte record. Model_BuildSlotAnimationTables accumulates one entry of
 * `values` per part and leaves `max` holding the largest of them, and
 * Model_ControlSlotAnimation, Model_UpdateSlotAnimation, Model_PlaySlotAnimationSounds and
 * Model_GetSlotAnimationLength read that same `max` as the length of animation
 * `row`. */
typedef struct {
    u16 values[MODEL_SLOT_PART_COUNT];
    u16 max;
} ModelSlotRow;

/* One entry of the slot's MODEL_SLOT_PART_COUNT-wide part table at 0x1E0:
 * the animation sequence driving one part of the model, which is libhmd's
 * GsSEQ. Model_LinkSlotAnimationUnit hands this table to GsLinkAnim as
 * GsSEQ ** and the library fills it in, and every field the game touches is
 * one GsSEQ names and uses the way libhmd does:
 *
 *   ii / aframe  Model_ControlSlotAnimation stores a command index into ii, and
 *                Model_LoadSlot clears both to 0xFFFF when it rearms a part
 *   sid / speed  Model_UpdateTintRequests in model_scene_controllers.c and
 *                model_slot_state_updates.c switch
 *                a part's sequence through sid; Model_SetSlotPartSpeeds sets speed
 *                for every part, and the rearm resets it to 0x10
 *   rframe..ti   Model_SeekSlotPartAnimation seeks a part by writing the frames left,
 *                total frames, current and target index
 *   start /      Model_BuildSlotAnimationTables reads the first command key
 *   start_sid    and stamps the row it resolved into start_sid, and the
 *                rearm restarts from both: ti = start, sid = start_sid
 *
 * Mirrored rather than taken from libhmd.h so this header stays free of the
 * libgte/libgpu/libgs/libhmd chain, as root_coordinate below does for
 * GsCOORDUNIT. model_slot_updates.c asserts the two layouts agree. */
typedef struct {
    u32 rewrite_idx;
    u16 size;
    u16 num;
    u16 ii;
    u16 aframe;
    u8 sid;
    s8 speed;
    u16 srcii;
    s16 rframe;
    u16 tframe;
    u16 ci;
    u16 ti;
    u16 start;
    u8 start_sid;
    u8 traveling;
} ModelSlotPart;

/* Row zero terminates; flags retains the packed cue phase and CD bit. */
typedef struct {
    u8 animation_row;
    u8 sound_id;
    u16 flags;
} ModelSlotSoundEntry;

/* Game-owned GsF_LIGHT view without importing the SDK header chain. */
typedef struct {
    s32 direction_x;
    s32 direction_y;
    s32 direction_z;
    u8 rgb[3];
    u8 pad_0F;
} ModelSlotLightEntry;

typedef struct {
    s32 field_00;
    s32 field_04;
    s32 field_08;
    s32 field_0C;
} ModelSlotS32Quad;

typedef union {
    struct {
        u8 field_00[0xA];
        u8 guardian_star_options[2];
    } bytes;
    struct {
        u16 field_00;
        u16 field_02;
        u8 field_04[4];
        u32 field_08;
    } values;
    /* The three per-axis minima Model_GetSlotDimensionsWithMinimums clamps an effect adjustment
     * up to, at slot offsets 0xCFF, 0xD00 and 0xD01. Each is a byte scaled
     * by 16, and zero means the axis is not clamped. A third arm rather than
     * members of `bytes` because that arm's field_00 is also the base of the
     * block move in model_slot_support.c. */
    struct {
        u8 pad_00[7];
        u8 min_x;
        u8 min_y;
        u8 min_z;
        u8 pad_0A[2];
    } thresholds;
} ModelSlotCF8Prefix;

typedef struct {
    ModelSlotCF8Prefix prefix;
    u16 battle_camera_duration_overrides[2];
    s32 field_10;
    s32 field_14;
    s32 field_18;
} ModelSlotCF8Block;

/* View rooted at the gModel_Slot0MetadataView interior alias and extending through the two
 * slot property bytes at DFE/DFF. The first words are compared by
 * Model_UpdateSlotControl; camera-yaw selection uses guardian_star_options
 * through guardian_star_selection. Other CF8 members retain relative
 * names where the payload's semantics are not established. */
typedef struct {
    ModelSlotCF8Prefix prefix;
    u16 battle_camera_duration_overrides[2];
    s32 field_10;
    s32 field_14;
    s32 field_18;
    u8 pad_1C[0xEA];
    u8 guardian_star_selection;
    u8 defense_position;
} ModelSlotCF8TailView;

/* A ModelSlotCF8Block's worth of words, for the one place that copies a whole
   block: file_transfer_steps.c's phase 10 fills field_CF8 straight out of the
   staged asset.

   This is a block-move spelling, not a second description of the block. The
   element type sets the alignment and the alignment sets the move width, so
   `u32` here and the halfword members of ModelSlotCF8Block do not lower the
   same way and the two are deliberately not interchangeable. The assert below
   is what ties them together. */
typedef struct {
    u32 value[7];
} ModelSlotCF8BlockWords;

/* The staged model metadata copied into a slot: 0x100 bytes of sound entries
   followed by the word-aligned field_CF8 block. */
typedef struct {
    ModelSlotSoundEntry sound_entries[MODEL_SLOT_SOUND_ENTRY_COUNT];
    ModelSlotCF8BlockWords field_CF8;
} ModelTransferMetadata;

/* Eight bytes moved as a block. Three units spelled this by hand over three
   different records: Model_UpdateBackgroundScroll and model_scene_controllers.c
   over the view snapshot pair gModel_wViewDistance/gModel_ViewSnapshot, which they had
   typed two different ways for the same two symbols, and model_slot_support.c over the halfword
   quad at ModelSlot.cached_dimensions.

   Unlike ModelSlotCF8BlockWords above, this one is shared rather than kept
   per site, and the element type is why. All three spellings were u8[8], so
   all three have alignment 1 and lower to the same move; two of them said
   __attribute__((packed)), which on a u8 array asks for the alignment it
   already has. A block type is interchangeable with another only when the
   element type agrees, which is what the note above is warning about. */
typedef struct {
    u8 bytes[8];
} ModelBytes8;

typedef struct {
    ModelSlotHeadEntry hmd_units[MODEL_SLOT_UNIT_COUNT];
    ModelSlotPart *G32 animation_sequences[MODEL_SLOT_PART_COUNT];
    /* Model_ResetSlotAnimationTables fills the keys with 0xFFFF at a 0x74
     * stride over MODEL_SLOT_ROW_COUNT rows. Model_BuildSlotAnimationTables
     * indexes them [row][part]; Model_ControlSlotAnimation reads the same
     * halfword as 0x2C8 + current * 116 + part * 2. */
    u16 animation_command_indices[MODEL_SLOT_ROW_COUNT][MODEL_SLOT_PART_COUNT];
    ModelSlotRow animation_duration_rows[MODEL_SLOT_ROW_COUNT];
    /* The part bitfield Model_ResetSlot's caller documents as "+0xBEC":
     * Model_ResetSlotAnimationTables sets bit `part % 8` of byte `part / 8`
     * from the command block. Model_ControlSlotAnimation reads it the same
     * way, and Model_LoadSlot widens a card from 0xC to 0x14 for flagged
     * parts. */
    u8 extended_animation_part_mask[8];
    u8 queued_animation_row;
    u8 current_animation_row;
    u8 queued_animation_row_latch;
    u8 field_BF7;
    ModelSlotSoundEntry sound_entries[MODEL_SLOT_SOUND_ENTRY_COUNT];
    ModelSlotCF8Block field_CF8;
    u8 *G32 entries;
    /* The slot's own placement unit: one 0x50-byte GsCOORDUNIT out of the
     * MODEL_SLOT_DATA_ENTRY_SIZE-stride run at `entries`. Left incomplete
     * here so this header stays free of the libgte/libgpu/libgs/libhmd
     * chain; sources that reach through it include "../psyq/libhmd.h". */
    struct _GsCOORDUNIT *G32 root_coordinate;
    /* The coordinate Model_InitSlot selects once the units are linked; see
     * that function's header comment. */
    struct _GsCOORDUNIT *G32 render_parent_coordinate;
    u8 pad_D20[0x50];
    ModelSlotLightEntry flat_lights[3];
    /* Signed ambient channels; retain the slot-2 negative-one sentinel. */
    s32 ambient_rgb_scale[3];
    u8 pad_DAC[4];
    /* Render-coordinate scale; preserve the fourth copy word. */
    ModelSlotS32Quad render_scale;
    /* Fade tint: [0..2] target RGB (0x80 neutral; 0 invisible in the
     * observed mode-5 fades, see Model_DrawSlot's zero-RGB skip), [3] blend
     * mode (0 opaque; 5 additive, the 3-D battle fade-in/out), [4..6] the
     * current RGB that Model_DrawSlot ramps toward [0..2] while [7] % 6 is
     * non-zero, [7] fade flag. [4..7] are static readings only. */
    u8 tint[8];
    /* Padded/smoothed origin spans; [3] is separate horizontal history. */
    u16 cached_dimensions[4];
    /* Cached XYZ and a separately derived Y value; copy all four. */
    s16 cached_position[4];
    /* The command list and the three pointers beside it, all installed by
     * Model_ResetSlotAnimationTables from the two blocks in the command
     * chain. Model_ControlSlotAnimation reads animation_commands as the base of
     * 4-byte command records and animation_parameter_base / animation_snapshot_buffer as its live-parameter base
     * and saved-word buffer. */
    s32 *G32 animation_commands;
    u8 *G32 animation_parameter_base;
    u8 *G32 animation_snapshot_buffer;
    u8 *G32 field_DE4;
    /* The module data words gModel_Slot0PrimaryDataAddress..gModel_Slot1VariantDataAddress Model_InitSlot copies in
     * for slots 0 and 1 (see high_memory_addresses.h). */
    s32 primary_module_data_address;
    s32 variant_module_data_address;
    /* Signed address words for the derived lit-color cache interval.
     * Partial resets can clear only its base; no live-interval invariant. */
    s32 lit_color_cache_address;
    s32 lit_color_cache_end_address;
    u16 model_id;
    u16 field_DFA;
    u16 field_DFC;
    u8 guardian_star_selection;
    u8 defense_position;
    /* Pre-cull packet-byte budget from the unit scan, not actual usage. */
    u16 packet_budget_bytes;
    /* Ordinary-unit scanned normal-index bound plus one; original u16 truncation. */
    u16 lit_color_cache_entry_count;
    /* Accepted decoder record counts, not rendered/visible polygons. */
    u16 prepared_primitive_count;
    /* 12.4 animation position; retain original u16 store truncation. */
    u16 animation_position;
    /* Model_ResetSlotAnimationTables clears this halfword beside animation_position
     * in src/game/model_slot_row_tables.c. */
    u16 animation_sound_position;
    /* Model_ResetSlot seeds this halfword with 0x1000 and
     * Model_LoadSlot reads it back; this unit's own header describes
     * the pair as "the halfword at +0xE0A". */
    u16 texture_tint_scale;
    /* Seeded 7 beside playback_speed's 8 by Model_ResetSlot and read back
     * by Model_LoadSlot, which its unit calls "the pair at
     * +0xE0C/+0xE0D". */
    /* Packed palette hue/invert control, not a Boolean or plain hue. */
    u8 texture_tint_control;
    u8 playback_speed;
    /* Control lifecycle and narrowed secondary-callback result share this byte. */
    u8 animation_control_state;
    /* Command/snapshot row, distinct from the current seek row. */
    u8 active_animation_command_row;
    /* Draw consumes this by clearing each sequence ii to 0xFFFF. */
    u8 part_command_reset_pending;
    /* Latched for drawing; mode 4 persists and forced draws restore it. */
    u8 draw_mode_request;
    /* Raw ground-shadow gate, not a normalized Boolean or slot visibility. */
    u8 shadow_enabled;
    /* Draw-driven stone protocol: 0xFF held, 0xFE finalization. */
    u8 stone_texture_state;
    /* Staged initialization step; 0 waits for transfer, 0xFF is idle. */
    u8 load_step;
    u8 field_E15;
    /* Raw codes: 0x3E forward, 0x3C reverse, 0x23 held. */
    u8 playback_state;
    u8 entry_count;
    u8 root_coordinate_index;
    u8 render_parent_coordinate_index;
    /* Low-byte HMD unit count capped by the existing initializer. */
    u8 hmd_unit_count;
    /* Number of linked animation-sequence pointers, not all HMD units. */
    u8 animation_sequence_count;
    u8 field_E1C;
    /* Raw packed-slot 0x80 setup option, not a Boolean or mute state. */
    u8 skip_sound_setup;
    /* Independent cue gate, also subject to the active/tint checks. */
    u8 animation_sound_enabled;
    /* Raw active gate; transient activation is separate from load readiness. */
    u8 is_active;
} ModelSlot;

typedef struct {
    s32 handler_value;
    s32 key;
} ModelHandlerRegistryEntry;

/* The camera move record at D_800F2B20. One leg per point the move drives --
 * the eye and the target -- and the two are laid out identically, which is
 * what makes the two halves of Model_StartCameraMove's setup literal copies of each
 * other. `slot` is the model slot the end point is read from and `pair_slot`
 * is the other slot of the pair; both are -1 when the point is not
 * slot-driven. */
typedef struct {
    s16 start_x;
    s16 start_y;
    s16 start_z;
    s16 pair_slot;
    s16 end_x;
    s16 end_y;
    s16 end_z;
    s16 slot;
} ModelCameraLeg;

/* `flags` bit 0 selects eye interpolation and bit 1 target interpolation;
 * either endpoint may use explicit coordinates instead of a model slot.
 * `duration` is twice the absolute duration the caller asked for, clamped to
 * 0xFFFF. Model_UpdateCamera tracks paired-slot X/Y and end-slot X/Y/Z by at most
 * 30 per update. The 0xFFFF sentinel skips elapsed advancement, not tracking
 * or interpolation. Model_CorrectViewDistance adjusts orbit_step using base_orbit_step as its
 * baseline; Model_UpdateCamera uses orbit_step for orbit yaw and decrements follow_countdown
 * in follow mode. */
typedef struct {
    u8 mode;
    u8 flags;
    u16 follow_countdown;
    u16 orbit_step;
#ifndef MODEL_CAMERA_MOVE_COMPACT_LAYOUT
    u16 base_orbit_step;
#endif
    u16 elapsed;
    u16 duration;
    ModelCameraLeg eye;
    ModelCameraLeg target;
} ModelCameraMove;

/* Four-byte tint descriptor, passed and copied whole. Only RGB is
 * interpolated; the start blend mode reaches the renderer unchanged.
 * The end blend mode is still copied but never read, not padding. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 blend_mode;
} ModelTintColor;

/* One of the MODEL_TINT_REQUEST_COUNT tint requests at gModel_aTintRequests.
 * Model_QueueTintRequest fills a free entry in; Model_UpdateTintRequests walks the table once a
 * frame, lerps `start` towards `end` by elapsed/duration, drops the result
 * into the model slot's tint, redraws through it and then restores
 * everything it touched.
 *
 * bit 0 of `flags` marks the entry live, bit 1 selects the model slot and
 * bits 3..7 carry the saved animation-sequence selector override.
 * animation_position_snapshot is the slot's animation_position when queued;
 * Model_UpdateTintRequests seeks the part sequences to it for the redraw. */
typedef struct {
    u16 flags;             /* 0x00 */
    u8 pad_02[8];          /* 0x02 */
    u16 animation_position_snapshot; /* 0x0A */
    u16 elapsed;           /* 0x0C */
    u16 duration;          /* 0x0E */
    ModelTintColor start;  /* 0x10 */
    ModelTintColor end;    /* 0x14 */
} ModelTintRequest;

typedef char ModelSlotHeadEntry_size_must_be_0x8[
    sizeof(ModelSlotHeadEntry) == 0x8 ? 1 : -1
];
typedef char ModelSlotHeadEntry_layout_must_match[
    MODEL_OFFSET(ModelSlotHeadEntry, coordinate_address) == 0 &&
    sizeof(((ModelSlotHeadEntry *)0)->coordinate_address) == 4 &&
    MODEL_OFFSET(ModelSlotHeadEntry, primitive_data) == 4 &&
    sizeof(((ModelSlotHeadEntry *)0)->primitive_data) == 4 ? 1 : -1
];
typedef char ModelSlot_hmd_units_extent_must_be_0x1E0[
    MODEL_OFFSET(ModelSlot, hmd_units) == 0 &&
    sizeof(((ModelSlot *)0)->hmd_units) == 0x1E0 ? 1 : -1
];
typedef char ModelSlotRow_size_must_be_0x76[
    sizeof(ModelSlotRow) == 0x76 ? 1 : -1
];
typedef char ModelSlotRow_max_offset_must_be_0x74[
    MODEL_OFFSET(ModelSlotRow, max) == 0x74 ? 1 : -1
];
typedef char ModelSlotPart_start_offset_must_be_0x18[
    MODEL_OFFSET(ModelSlotPart, start) == 0x18 ? 1 : -1
];
typedef char ModelSlot_animation_command_indices_offset_must_be_0x2C8[
    MODEL_OFFSET(ModelSlot, animation_command_indices) ==
        MODEL_SLOT_ROW_KEY_TABLE_OFFSET ? 1 : -1
];
typedef char ModelSlot_animation_duration_rows_offset_must_be_0x750[
    MODEL_OFFSET(ModelSlot, animation_duration_rows) == 0x750 ? 1 : -1
];
typedef char ModelSlot_extended_animation_part_mask_offset_must_be_0xBEC[
    MODEL_OFFSET(ModelSlot, extended_animation_part_mask) == 0xBEC &&
    sizeof(((ModelSlot *)0)->extended_animation_part_mask) == 8 ? 1 : -1
];
typedef char ModelSlot_animation_commands_offset_must_be_0xDD8[
    MODEL_OFFSET(ModelSlot, animation_commands) == 0xDD8 ? 1 : -1
];
typedef char ModelSlot_animation_control_state_offset_must_be_0xE0E[
    MODEL_OFFSET(ModelSlot, animation_control_state) == 0xE0E ? 1 : -1
];
typedef char ModelSlotCF8Block_size_must_be_0x1C[
    sizeof(ModelSlotCF8Block) == 0x1C ? 1 : -1
];
typedef char ModelSlotCF8BlockWords_size_must_match_block[
    sizeof(ModelSlotCF8BlockWords) == sizeof(ModelSlotCF8Block) ? 1 : -1
];
typedef char ModelTransferMetadata_field_CF8_offset_must_be_0x100[
    MODEL_OFFSET(ModelTransferMetadata, field_CF8) == 0x100 ? 1 : -1
];
typedef char ModelBytes8_size_must_be_8[
    sizeof(ModelBytes8) == 8 ? 1 : -1
];
typedef char ModelSlotCF8Block_guardian_star_options_offset_must_be_0xA[
    MODEL_OFFSET(ModelSlotCF8Block, prefix.bytes.guardian_star_options) == 0xA ? 1 : -1
];
typedef char ModelSlotCF8Block_battle_camera_duration_overrides_offset_must_be_0xC[
    MODEL_OFFSET(ModelSlotCF8Block, battle_camera_duration_overrides) == 0xC ? 1 : -1
];
typedef char ModelSlotLightEntry_size_must_be_0x10[
    sizeof(ModelSlotLightEntry) == 0x10 ? 1 : -1
];
typedef char ModelSlotLightEntry_layout_must_match[
    MODEL_OFFSET(ModelSlotLightEntry, direction_x) == 0 &&
    MODEL_OFFSET(ModelSlotLightEntry, direction_y) == 4 &&
    MODEL_OFFSET(ModelSlotLightEntry, direction_z) == 8 &&
    MODEL_OFFSET(ModelSlotLightEntry, rgb) == 0xC &&
    sizeof(((ModelSlotLightEntry *)0)->rgb) == 3 &&
    MODEL_OFFSET(ModelSlotLightEntry, pad_0F) == 0xF ? 1 : -1
];
typedef char ModelSlotS32Quad_size_must_be_0x10[
    sizeof(ModelSlotS32Quad) == 0x10 ? 1 : -1
];
typedef char ModelSlotSoundEntry_size_must_be_4[
    sizeof(ModelSlotSoundEntry) == 4 ? 1 : -1
];
typedef char ModelSlot_size_must_be_0xE20[
    sizeof(ModelSlot) == MODEL_SLOT_SIZE ? 1 : -1
];
typedef char ModelSlot_animation_sequences_offset_must_be_0x1E0[
    MODEL_OFFSET(ModelSlot, animation_sequences) == 0x1E0 ? 1 : -1
];
typedef char ModelSlot_animation_duration_rows_max_offset_must_be_0x7C4[
    MODEL_OFFSET(ModelSlot, animation_duration_rows[0].max) == 0x7C4 ? 1 : -1
];
typedef char ModelSlot_current_animation_row_offset_must_be_0xBF5[
    MODEL_OFFSET(ModelSlot, current_animation_row) == 0xBF5 ? 1 : -1
];
typedef char ModelSlot_animation_control_layout_must_match[
    MODEL_OFFSET(ModelSlot, queued_animation_row) == 0xBF4 &&
    sizeof(((ModelSlot *)0)->queued_animation_row) == 1 &&
    sizeof(((ModelSlot *)0)->current_animation_row) == 1 &&
    MODEL_OFFSET(ModelSlot, queued_animation_row_latch) == 0xBF6 &&
    sizeof(((ModelSlot *)0)->queued_animation_row_latch) == 1 &&
    MODEL_OFFSET(ModelSlot, animation_sound_position) == 0xE08 &&
    sizeof(((ModelSlot *)0)->animation_sound_position) == 2 &&
    sizeof(((ModelSlot *)0)->playback_speed) == 1 ? 1 : -1
];
typedef char ModelSlotSoundEntry_layout_must_match[
    sizeof(ModelSlotSoundEntry) == 4 &&
    MODEL_OFFSET(ModelSlotSoundEntry, animation_row) == 0 &&
    sizeof(((ModelSlotSoundEntry *)0)->animation_row) == 1 &&
    MODEL_OFFSET(ModelSlotSoundEntry, sound_id) == 1 &&
    sizeof(((ModelSlotSoundEntry *)0)->sound_id) == 1 &&
    MODEL_OFFSET(ModelSlotSoundEntry, flags) == 2 &&
    sizeof(((ModelSlotSoundEntry *)0)->flags) == 2 ? 1 : -1
];
typedef char ModelSlot_sound_entries_offset_must_be_0xBF8[
    MODEL_OFFSET(ModelSlot, sound_entries) == 0xBF8 ? 1 : -1
];
typedef char ModelSlot_field_CF8_offset_must_be_0xCF8[
    MODEL_OFFSET(ModelSlot, field_CF8) == 0xCF8 ? 1 : -1
];
typedef char ModelSlot_entries_offset_must_be_0xD14[
    MODEL_OFFSET(ModelSlot, entries) == 0xD14 ? 1 : -1
];
typedef char ModelSlot_root_coordinate_offset_must_be_0xD18[
    MODEL_OFFSET(ModelSlot, root_coordinate) == 0xD18 ? 1 : -1
];
typedef char ModelSlot_render_parent_coordinate_offset_must_be_0xD1C[
    MODEL_OFFSET(ModelSlot, render_parent_coordinate) == 0xD1C ? 1 : -1
];
typedef char ModelSlot_coordinate_pointer_layout_must_match[
    MODEL_OFFSET(ModelSlot, root_coordinate) == 0xD18 &&
    sizeof(((ModelSlot *)0)->root_coordinate) == 4 &&
    MODEL_OFFSET(ModelSlot, render_parent_coordinate) == 0xD1C &&
    sizeof(((ModelSlot *)0)->render_parent_coordinate) == 4 ? 1 : -1
];
typedef char ModelSlot_flat_lights_offset_must_be_0xD70[
    MODEL_OFFSET(ModelSlot, flat_lights) == 0xD70 &&
    sizeof(((ModelSlot *)0)->flat_lights) == 0x30 ? 1 : -1
];
typedef char ModelSlot_ambient_rgb_scale_offset_must_be_0xDA0[
    MODEL_OFFSET(ModelSlot, ambient_rgb_scale) == 0xDA0 ? 1 : -1
];
typedef char ModelSlot_render_scale_offset_must_be_0xDB0[
    MODEL_OFFSET(ModelSlot, render_scale) == 0xDB0 ? 1 : -1
];
typedef char ModelSlot_field_DC0_offset_must_be_0xDC0[
    MODEL_OFFSET(ModelSlot, tint) == 0xDC0 ? 1 : -1
];
typedef char ModelSlot_cached_dimensions_offset_must_be_0xDC8[
    MODEL_OFFSET(ModelSlot, cached_dimensions) == 0xDC8 ? 1 : -1
];
typedef char ModelSlot_scale_dimension_layout_must_match[
    MODEL_OFFSET(ModelSlot, render_scale) == 0xDB0 &&
    sizeof(((ModelSlot *)0)->render_scale) == 0x10 &&
    MODEL_OFFSET(ModelSlot, cached_dimensions) == 0xDC8 &&
    sizeof(((ModelSlot *)0)->cached_dimensions) == 8 &&
    sizeof(((ModelSlot *)0)->cached_dimensions[0]) == 2 ? 1 : -1
];
typedef char ModelSlot_cached_position_offset_must_be_0xDD0[
    MODEL_OFFSET(ModelSlot, cached_position) == 0xDD0 ? 1 : -1
];
typedef char ModelSlot_cached_position_width_must_match[
    sizeof(((ModelSlot *)0)->cached_position) == 8 &&
    sizeof(((ModelSlot *)0)->cached_position[0]) == 2 ? 1 : -1
];
typedef char ModelSlot_animation_snapshot_pointer_layout_must_match[
    MODEL_OFFSET(ModelSlot, animation_parameter_base) == 0xDDC &&
    sizeof(((ModelSlot *)0)->animation_parameter_base) == 4 &&
    MODEL_OFFSET(ModelSlot, animation_snapshot_buffer) == 0xDE0 &&
    sizeof(((ModelSlot *)0)->animation_snapshot_buffer) == 4 ? 1 : -1
];
typedef char ModelSlot_primary_module_data_address_offset_must_be_0xDE8[
    MODEL_OFFSET(ModelSlot, primary_module_data_address) == 0xDE8 ? 1 : -1
];
typedef char ModelSlot_variant_module_data_address_offset_must_be_0xDEC[
    MODEL_OFFSET(ModelSlot, variant_module_data_address) == 0xDEC ? 1 : -1
];
typedef char ModelSlot_module_data_address_layout_must_match[
    MODEL_OFFSET(ModelSlot, primary_module_data_address) == 0xDE8 &&
    sizeof(((ModelSlot *)0)->primary_module_data_address) == 4 &&
    MODEL_OFFSET(ModelSlot, variant_module_data_address) == 0xDEC &&
    sizeof(((ModelSlot *)0)->variant_module_data_address) == 4 ? 1 : -1
];
typedef char ModelSlot_model_id_offset_must_be_0xDF8[
    MODEL_OFFSET(ModelSlot, model_id) == 0xDF8 ? 1 : -1
];
typedef char ModelSlot_field_DFA_offset_must_be_0xDFA[
    MODEL_OFFSET(ModelSlot, field_DFA) == 0xDFA ? 1 : -1
];
typedef char ModelSlot_field_DFC_offset_must_be_0xDFC[
    MODEL_OFFSET(ModelSlot, field_DFC) == 0xDFC ? 1 : -1
];
typedef char ModelSlot_guardian_star_selection_offset_must_be_0xDFE[
    MODEL_OFFSET(ModelSlot, guardian_star_selection) == 0xDFE ? 1 : -1
];
typedef char ModelSlot_defense_position_offset_must_be_0xDFF[
    MODEL_OFFSET(ModelSlot, defense_position) == 0xDFF ? 1 : -1
];
typedef char ModelSlot_battle_property_layout_must_match[
    MODEL_OFFSET(ModelSlot, model_id) == 0xDF8 &&
    sizeof(((ModelSlot *)0)->model_id) == 2 &&
    MODEL_OFFSET(ModelSlot, guardian_star_selection) == 0xDFE &&
    sizeof(((ModelSlot *)0)->guardian_star_selection) == 1 &&
    MODEL_OFFSET(ModelSlot, defense_position) == 0xDFF &&
    sizeof(((ModelSlot *)0)->defense_position) == 1 ? 1 : -1
];
typedef char ModelSlot_packet_budget_bytes_offset_must_be_0xE00[
    MODEL_OFFSET(ModelSlot, packet_budget_bytes) == 0xE00 ? 1 : -1
];
typedef char ModelSlot_prepared_primitive_count_offset_must_be_0xE04[
    MODEL_OFFSET(ModelSlot, prepared_primitive_count) == 0xE04 ? 1 : -1
];
typedef char ModelSlot_animation_position_offset_must_be_0xE06[
    MODEL_OFFSET(ModelSlot, animation_position) == 0xE06 ? 1 : -1
];
typedef char ModelSlot_playback_speed_offset_must_be_0xE0D[
    MODEL_OFFSET(ModelSlot, playback_speed) == 0xE0D ? 1 : -1
];
typedef char ModelSlot_draw_mode_request_offset_must_be_0xE11[
    MODEL_OFFSET(ModelSlot, draw_mode_request) == 0xE11 ? 1 : -1
];
typedef char ModelSlot_shadow_enabled_offset_must_be_0xE12[
    MODEL_OFFSET(ModelSlot, shadow_enabled) == 0xE12 ? 1 : -1
];
typedef char ModelSlot_post_draw_state_layout_must_match[
    MODEL_OFFSET(ModelSlot, shadow_enabled) == 0xE12 &&
    sizeof(((ModelSlot *)0)->shadow_enabled) == 1 &&
    MODEL_OFFSET(ModelSlot, stone_texture_state) == 0xE13 &&
    sizeof(((ModelSlot *)0)->stone_texture_state) == 1 ? 1 : -1
];
typedef char ModelSlot_load_step_offset_must_be_0xE14[
    MODEL_OFFSET(ModelSlot, load_step) == 0xE14 ? 1 : -1
];
typedef char ModelSlot_playback_state_offset_must_be_0xE16[
    MODEL_OFFSET(ModelSlot, playback_state) == 0xE16 ? 1 : -1
];
typedef char ModelSlot_animation_control_state_layout_must_match[
    MODEL_OFFSET(ModelSlot, animation_control_state) == 0xE0E &&
    sizeof(((ModelSlot *)0)->animation_control_state) == 1 &&
    MODEL_OFFSET(ModelSlot, active_animation_command_row) == 0xE0F &&
    sizeof(((ModelSlot *)0)->active_animation_command_row) == 1 &&
    MODEL_OFFSET(ModelSlot, part_command_reset_pending) == 0xE10 &&
    sizeof(((ModelSlot *)0)->part_command_reset_pending) == 1 &&
    sizeof(((ModelSlot *)0)->playback_state) == 1 ? 1 : -1
];
typedef char ModelSlot_entry_count_offset_must_be_0xE17[
    MODEL_OFFSET(ModelSlot, entry_count) == 0xE17 ? 1 : -1
];
typedef char ModelSlot_root_coordinate_index_offset_must_be_0xE18[
    MODEL_OFFSET(ModelSlot, root_coordinate_index) == 0xE18 ? 1 : -1
];
typedef char ModelSlot_coordinate_index_layout_must_match[
    MODEL_OFFSET(ModelSlot, root_coordinate_index) == 0xE18 &&
    sizeof(((ModelSlot *)0)->root_coordinate_index) == 1 &&
    MODEL_OFFSET(ModelSlot, render_parent_coordinate_index) == 0xE19 &&
    sizeof(((ModelSlot *)0)->render_parent_coordinate_index) == 1 ? 1 : -1
];
typedef char ModelSlot_hmd_unit_count_offset_must_be_0xE1A[
    MODEL_OFFSET(ModelSlot, hmd_unit_count) == 0xE1A ? 1 : -1
];
typedef char ModelSlot_animation_sequence_count_offset_must_be_0xE1B[
    MODEL_OFFSET(ModelSlot, animation_sequence_count) == 0xE1B ? 1 : -1
];
typedef char ModelSlot_field_E1C_offset_must_be_0xE1C[
    MODEL_OFFSET(ModelSlot, field_E1C) == 0xE1C ? 1 : -1
];
typedef char ModelSlot_animation_sound_enabled_offset_must_be_0xE1E[
    MODEL_OFFSET(ModelSlot, animation_sound_enabled) == 0xE1E ? 1 : -1
];
typedef char ModelSlot_sound_gate_layout_must_match[
    MODEL_OFFSET(ModelSlot, skip_sound_setup) == 0xE1D &&
    sizeof(((ModelSlot *)0)->skip_sound_setup) == 1 &&
    MODEL_OFFSET(ModelSlot, animation_sound_enabled) == 0xE1E &&
    sizeof(((ModelSlot *)0)->animation_sound_enabled) == 1 ? 1 : -1
];
typedef char ModelSlot_is_active_offset_must_be_0xE1F[
    MODEL_OFFSET(ModelSlot, is_active) == 0xE1F ? 1 : -1
];

typedef char ModelSlotCF8TailView_size_must_be_0x108[
    sizeof(ModelSlotCF8TailView) == 0x108 ? 1 : -1
];
typedef char ModelSlotCF8TailView_guardian_star_options_offset_must_be_0xA[
    MODEL_OFFSET(ModelSlotCF8TailView, prefix.bytes.guardian_star_options) == 0xA ? 1 : -1
];
typedef char ModelSlotCF8Prefix_size_must_be_0xC[
    sizeof(ModelSlotCF8Prefix) == 0xC ? 1 : -1
];
typedef char ModelSlotCF8Prefix_field_08_offset_must_be_0x8[
    MODEL_OFFSET(ModelSlotCF8Prefix, values.field_08) == 0x8 ? 1 : -1
];
typedef char ModelSlotCF8Block_field_10_offset_must_be_0x10[
    MODEL_OFFSET(ModelSlotCF8Block, field_10) == 0x10 ? 1 : -1
];
typedef char ModelSlotCF8Block_field_14_offset_must_be_0x14[
    MODEL_OFFSET(ModelSlotCF8Block, field_14) == 0x14 ? 1 : -1
];
typedef char ModelSlotCF8Block_field_18_offset_must_be_0x18[
    MODEL_OFFSET(ModelSlotCF8Block, field_18) == 0x18 ? 1 : -1
];
typedef char ModelSlotCF8TailView_guardian_star_selection_offset_must_be_0x106[
    MODEL_OFFSET(ModelSlotCF8TailView, guardian_star_selection) ==
        MODEL_SLOT_CF8_DFE_OFFSET ? 1 : -1
];
typedef char ModelSlotCF8TailView_defense_position_offset_must_be_0x107[
    MODEL_OFFSET(ModelSlotCF8TailView, defense_position) ==
        MODEL_SLOT_CF8_DFF_OFFSET ? 1 : -1
];

typedef char ModelSlotCF8_guardian_star_metadata_layout_must_match[
    sizeof(((ModelSlotCF8Prefix *)0)->bytes.guardian_star_options) == 2 &&
    MODEL_OFFSET(ModelSlotCF8Block, battle_camera_duration_overrides) == 0xC &&
    sizeof(((ModelSlotCF8Block *)0)->battle_camera_duration_overrides) == 4 &&
    MODEL_OFFSET(ModelSlotCF8TailView, battle_camera_duration_overrides) == 0xC &&
    sizeof(((ModelSlotCF8TailView *)0)->battle_camera_duration_overrides) == 4 &&
    sizeof(((ModelSlotCF8TailView *)0)->guardian_star_selection) == 1 &&
    sizeof(((ModelSlotCF8TailView *)0)->defense_position) == 1 &&
    MODEL_OFFSET(ModelSlot, field_CF8) +
        MODEL_OFFSET(ModelSlotCF8TailView, guardian_star_selection) ==
        MODEL_OFFSET(ModelSlot, guardian_star_selection) &&
    MODEL_OFFSET(ModelSlot, field_CF8) +
        MODEL_OFFSET(ModelSlotCF8TailView, defense_position) ==
        MODEL_OFFSET(ModelSlot, defense_position) ? 1 : -1
];

typedef char ModelHandlerRegistryEntry_size_must_be_0x8[
    sizeof(ModelHandlerRegistryEntry) == 0x8 ? 1 : -1
];
typedef char ModelHandlerRegistryEntry_handler_value_offset_must_be_0x0[
    MODEL_OFFSET(ModelHandlerRegistryEntry, handler_value) == 0x0 ? 1 : -1
];
typedef char ModelHandlerRegistryEntry_key_offset_must_be_0x4[
    MODEL_OFFSET(ModelHandlerRegistryEntry, key) == 0x4 ? 1 : -1
];

typedef char ModelCameraLeg_size_must_be_0x10[
    sizeof(ModelCameraLeg) == 0x10 ? 1 : -1
];
typedef char ModelCameraLeg_pair_slot_offset_must_be_0x6[
    MODEL_OFFSET(ModelCameraLeg, pair_slot) == 0x6 ? 1 : -1
];
typedef char ModelCameraLeg_end_x_offset_must_be_0x8[
    MODEL_OFFSET(ModelCameraLeg, end_x) == 0x8 ? 1 : -1
];
typedef char ModelCameraLeg_slot_offset_must_be_0xE[
    MODEL_OFFSET(ModelCameraLeg, slot) == 0xE ? 1 : -1
];

typedef char ModelCameraMove_control_prefix_must_match[
    MODEL_OFFSET(ModelCameraMove, follow_countdown) == 2 &&
    sizeof(((ModelCameraMove *)0)->follow_countdown) == 2 &&
    MODEL_OFFSET(ModelCameraMove, orbit_step) == 4 &&
    sizeof(((ModelCameraMove *)0)->orbit_step) == 2 ? 1 : -1
];
#ifndef MODEL_CAMERA_MOVE_COMPACT_LAYOUT
typedef char ModelCameraMove_baseline_word_must_match[
    MODEL_OFFSET(ModelCameraMove, base_orbit_step) == 6 &&
    sizeof(((ModelCameraMove *)0)->base_orbit_step) == 2 ? 1 : -1
];
#endif

#ifdef MODEL_CAMERA_MOVE_COMPACT_LAYOUT
typedef char ModelCameraMove_size_must_be_0x2A[
    sizeof(ModelCameraMove) == 0x2A ? 1 : -1
];
typedef char ModelCameraMove_duration_offset_must_be_0x8[
    MODEL_OFFSET(ModelCameraMove, duration) == 0x8 ? 1 : -1
];
typedef char ModelCameraMove_eye_offset_must_be_0xA[
    MODEL_OFFSET(ModelCameraMove, eye) == 0xA ? 1 : -1
];
typedef char ModelCameraMove_target_offset_must_be_0x1A[
    MODEL_OFFSET(ModelCameraMove, target) == 0x1A ? 1 : -1
];
#else
typedef char ModelCameraMove_size_must_be_0x2C[
    sizeof(ModelCameraMove) == 0x2C ? 1 : -1
];
typedef char ModelCameraMove_duration_offset_must_be_0xA[
    MODEL_OFFSET(ModelCameraMove, duration) == 0xA ? 1 : -1
];
typedef char ModelCameraMove_eye_offset_must_be_0xC[
    MODEL_OFFSET(ModelCameraMove, eye) == 0xC ? 1 : -1
];
typedef char ModelCameraMove_target_offset_must_be_0x1C[
    MODEL_OFFSET(ModelCameraMove, target) == 0x1C ? 1 : -1
];
#endif

/* One entry of the eight-byte table at gModel_aCameraOrbitPresets.  Every access in the tree
 * is sixteen bits wide: Model_QueueCameraOrbitFromPreset reads field_00, Model_SetCameraOrbitFromPreset reads
 * field_00, angle and field_04 of one record, and Model_GetSlotCameraPresetYaw reads
 * `angle` and wraps it modulo a full turn.  The retail bytes agree -- the
 * first entries are 02BC / FE00 / FF00 / 0000 and 02BC / 0200 / FF00 / 0000,
 * where field_00 is a constant 700 and the second halfword steps in eighths
 * of a turn.  Nothing reads the record as two 32-bit words.
 *
 * The table is fifteen records: it ends at 0x800915E8, where the next object
 * begins and has none of this pattern.  The declaration is left unsized
 * anyway, because Model_GetSlotCameraPresetYaw masks its index with 0x1F and so can reach
 * past the fifteenth -- the mask is not an entry count, and a bound here
 * would say otherwise.
 */
typedef struct {
    s16 field_00;
    s16 angle;
    s16 field_04;
    s16 field_06;
} ModelEffectCoefficient;

typedef char ModelTintColor_size_must_be_0x4[
    sizeof(ModelTintColor) == 0x4 ? 1 : -1
];
typedef char ModelTintColor_layout_must_match[
    MODEL_OFFSET(ModelTintColor, r) == 0 &&
    sizeof(((ModelTintColor *)0)->r) == 1 &&
    MODEL_OFFSET(ModelTintColor, g) == 1 &&
    sizeof(((ModelTintColor *)0)->g) == 1 &&
    MODEL_OFFSET(ModelTintColor, b) == 2 &&
    sizeof(((ModelTintColor *)0)->b) == 1 &&
    MODEL_OFFSET(ModelTintColor, blend_mode) == 3 &&
    sizeof(((ModelTintColor *)0)->blend_mode) == 1 ? 1 : -1
];
typedef char ModelTintRequest_size_must_be_0x18[
    sizeof(ModelTintRequest) == 0x18 ? 1 : -1
];
typedef char ModelTintRequest_animation_position_snapshot_layout_must_match[
    MODEL_OFFSET(ModelTintRequest, animation_position_snapshot) == 0xA &&
    sizeof(((ModelTintRequest *)0)->animation_position_snapshot) == 2 ? 1 : -1
];
typedef char ModelTintRequest_elapsed_offset_must_be_0xC[
    MODEL_OFFSET(ModelTintRequest, elapsed) == 0xC ? 1 : -1
];
typedef char ModelTintRequest_start_offset_must_be_0x10[
    MODEL_OFFSET(ModelTintRequest, start) == 0x10 ? 1 : -1
];
typedef char ModelTintRequest_end_offset_must_be_0x14[
    MODEL_OFFSET(ModelTintRequest, end) == 0x14 ? 1 : -1
];

typedef char ModelEffectCoefficient_size_must_be_0x8[
    sizeof(ModelEffectCoefficient) == 0x8 ? 1 : -1
];
typedef char ModelEffectCoefficient_angle_offset_must_be_0x2[
    MODEL_OFFSET(ModelEffectCoefficient, angle) == 0x2 ? 1 : -1
];
typedef char ModelEffectCoefficient_field_04_offset_must_be_0x4[
    MODEL_OFFSET(ModelEffectCoefficient, field_04) == 0x4 ? 1 : -1
];

#undef MODEL_OFFSET

#ifndef MODEL_EFFECT_COEFFICIENT_CUSTOM_EXTERN
extern ModelEffectCoefficient gModel_aCameraOrbitPresets[];
#endif
#ifndef MODEL_SLOT_CUSTOM_EXTERN
extern ModelSlot gModel_aSlots[MODEL_SLOT_COUNT];

/* Interior names for fields of slot zero. Their types come from the asserted
 * ModelSlot layout above; consumers still step their addresses by
 * MODEL_SLOT_SIZE when selecting another slot, so none of these declarations
 * claims that same-named fields from adjacent slots are contiguous arrays.
 *
 * 0x800F3938 = gModel_aSlots[0].field_CF8
 * 0x800F39B0 = gModel_aSlots[0].flat_lights
 * 0x800F39F0 = gModel_aSlots[0].render_scale
 * 0x800F3A10 = gModel_aSlots[0].cached_position
 *
 * For example, the cached_position consumers use the equivalent byte-stride form
 *
 *     entry = (u8 *)gModel_awSlot0Position + index * MODEL_SLOT_SIZE;
 *
 * to reach that field of slot `index`. model_distance_queries.c reads the
 * first three halfwords of it and differences them against gModel_RView
 * before SquareRoot0, so they are a position.
 *
 * The two names stay separate: all three matched sites reach the field
 * through this symbol, so writing it as an offset from gModel_aSlots would
 * change which symbol their relocations name. Unlike the selection-table case
 * there is no assembly reader to corroborate that, but the matched C is
 * itself the evidence. */
extern ModelSlotCF8TailView gModel_Slot0MetadataView;
extern ModelSlotLightEntry gModel_aSlot0FlatLights[3];
extern ModelSlotS32Quad gModel_Slot0Scale;
extern u16 gModel_awSlot0Position[];
#endif
#ifndef MODEL_CAMERA_MOVE_CUSTOM_EXTERN
extern ModelCameraMove D_800F2B20;
#endif
#ifndef MODEL_TINT_REQUEST_CUSTOM_EXTERN
extern ModelTintRequest gModel_aTintRequests[MODEL_TINT_REQUEST_COUNT];
#endif

extern ModelHandlerRegistryEntry
    tent_ModelHandlerRegistry[MODEL_HANDLER_REGISTRY_COUNT];
extern ModelHandlerRegistryEntry gModel_aHandlerKeyRegistry[MODEL_HANDLER_REGISTRY_COUNT] __asm__("tent_ModelHandlerRegistry");

s32 Model_LoadMonsterMerge(
    s32 slot, s32 model, s32 p2, s32 p3, s32 p4, s32 p5, s32 arg6
);
void Model_SetSlotDrawMode(s32 index, s32 value);

#endif
