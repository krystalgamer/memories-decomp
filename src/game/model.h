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
#define MODEL_SLOT_ROW_COUNT 10
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

typedef struct {
    u32 field_00;
    void *field_04;
} ModelSlotHeadEntry;

/* One of the MODEL_SLOT_ROW_COUNT accumulator rows at slot offset 0x750.
 * func_8004D58C's reset walks the rows and their maxima in the same loop
 * iteration -- 58 halfwords at 0x750 + row * 0x76 and the halfword at
 * 0x7C4 + row * 0x76 -- which is what groups `values` and `max` into one
 * 0x76-byte record. func_8004D75C then accumulates one entry of `values`
 * per part and leaves `max` holding the largest of them, and func_80057AF4,
 * func_800556E8, func_8005106C and func_80058EC0 read that same `max` as the
 * length of animation `row`. */
typedef struct {
    u16 values[MODEL_SLOT_PART_COUNT];
    u16 max;
} ModelSlotRow;

/* One entry of the slot's MODEL_SLOT_PART_COUNT-wide part table at 0x1E0.
 * Only the fields the model code reaches are named: func_80057AF4 stores the
 * part's current source index at +0x08, func_800528AC pushes a part id
 * through +0x0C, func_8005611C copies +0x18 down to +0x16, and func_8004D75C
 * reads +0x18 as the part's first command key while stamping the row it was
 * resolved on into +0x1A. */
typedef struct {
    u8 pad_00[8];
    u16 field_08;
    u8 pad_0A[2];
    u8 field_0C;
    u8 pad_0D[9];
    u16 field_16;
    u16 field_18;
    u8 field_1A;
} ModelSlotPart;

typedef struct {
    u8 frame;
    u8 id;
    u16 flags;
} ModelSlotSoundEntry;

typedef struct {
    s32 field_00;
    s32 field_04;
    s32 field_08;
    u8 field_0C[3];
    u8 pad_0F;
} ModelSlotLightEntry;

typedef struct {
    s32 field_00;
    s32 field_04;
    s32 field_08;
    s32 field_0C;
} ModelSlotS32Quad;

typedef struct {
    u8 field_00[0xA];
    u8 field_0A[2];
    u16 field_0C[8];
} ModelSlotCF8Block;

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

/* Eight bytes moved as a block. Three units spelled this by hand over three
   different records: func_8004E7B0 and model_scene_setup.c over the view
   snapshot pair D_8009B478/D_8009B480, which they had typed two different
   ways for the same two symbols, and model_slot_support.c over the halfword
   quad at ModelSlot.field_DC8.

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
    ModelSlotHeadEntry field_000[1];
    u8 pad_008[0x1D8];
    ModelSlotPart *field_1E0[MODEL_SLOT_PART_COUNT];
    /* The per-animation key table. func_8004D58C fills it with 0xFFFF at a
     * 0x74 stride over MODEL_SLOT_ROW_COUNT rows, func_8004D75C indexes it as
     * [row][part], and func_80057AF4 reads the same halfword through the
     * literal offset arithmetic 0x2C8 + current * 116 + part * 2. */
    u16 field_2C8[MODEL_SLOT_ROW_COUNT][MODEL_SLOT_PART_COUNT];
    ModelSlotRow field_750[MODEL_SLOT_ROW_COUNT];
    /* The part bitfield func_8005611C's caller documents as "+0xBEC":
     * func_8004D58C sets bit `part % 8` of byte `part / 8` from the command
     * block, func_80057AF4 reads it back the same way, and func_80056250
     * widens a card from 0xC to 0x14 for the parts it flags. */
    u8 field_BEC[8];
    u8 field_BF4;
    u8 field_BF5;
    u8 field_BF6;
    u8 field_BF7;
    ModelSlotSoundEntry sound_entries[MODEL_SLOT_SOUND_ENTRY_COUNT];
    ModelSlotCF8Block field_CF8;
    u8 *entries;
    /* The slot's own placement unit: one 0x50-byte GsCOORDUNIT out of the
     * MODEL_SLOT_DATA_ENTRY_SIZE-stride run at `entries`. Left incomplete
     * here so this header stays free of the libgte/libgpu/libgs/libhmd
     * chain; sources that reach through it include "../psyq/libhmd.h". */
    struct _GsCOORDUNIT *field_D18;
    u8 pad_D1C[0x54];
    ModelSlotLightEntry field_D70[3];
    s32 field_DA0[3];
    u8 pad_DAC[4];
    ModelSlotS32Quad field_DB0;
    u8 field_DC0[8];
    u16 field_DC8[4];
    u16 field_DD0[4];
    /* The command list and the three pointers beside it, all installed by
     * func_8004D58C out of the two blocks it finds in the command chain.
     * func_80057AF4 reads field_DD8 as the base of 4-byte command records and
     * field_DDC / field_DE0 as the source and destination of its transfers. */
    s32 *field_DD8;
    u8 *field_DDC;
    u8 *field_DE0;
    u8 *field_DE4;
    u8 pad_DE8[0x10];
    u16 field_DF8;
    u16 field_DFA;
    u16 field_DFC;
    u8 field_DFE;
    u8 field_DFF;
    u8 pad_E00[6];
    u16 field_E06;
    u8 pad_E08[5];
    u8 field_E0D;
    u8 field_E0E;
    u8 field_E0F;
    u8 field_E10;
    u8 field_E11;
    u8 field_E12;
    u8 field_E13;
    u8 field_E14;
    u8 field_E15;
    u8 field_E16;
    u8 entry_count;
    u8 field_E18;
    u8 field_E19;
    u8 field_E1A;
    u8 field_E1B;
    u8 pad_E1C[3];
    u8 field_E1F;
} ModelSlot;

typedef struct {
    s32 handler_value;
    s32 key;
} ModelHandlerRegistryEntry;

/* The camera move record at D_800F2B20. One leg per point the move drives --
 * the eye and the target -- and the two are laid out identically, which is
 * what makes the two halves of func_80052D2C's setup literal copies of each
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

/* `flags` bit 0 marks the eye as slot-driven and bit 1 the target;
 * `duration` is twice the absolute duration the caller asked for, clamped to
 * 0xFFFF. field_02, field_04 and field_06 are the easing counters
 * func_80052694 walks. */
typedef struct {
    u8 mode;
    u8 flags;
    u16 field_02;
    u16 field_04;
    u16 field_06;
    u16 elapsed;
    u16 duration;
    ModelCameraLeg eye;
    ModelCameraLeg target;
} ModelCameraMove;

/* One end of a tint ramp. func_80058938 takes a whole one by value and stores
 * it as a single word, so all four bytes are live even though only b0..b2 are
 * the colour: func_800528AC interpolates those three and copies b3 of the
 * start colour straight through as the part id it draws with. The end
 * colour's b3 at +0x17 is written and never read, which is why it is a member
 * here and not padding -- func_800528AC used to call it pad_17. */
typedef struct {
    u8 b0;
    u8 b1;
    u8 b2;
    u8 b3;
} ModelTintColor;

/* One of the MODEL_TINT_REQUEST_COUNT tint requests at D_800F2B50.
 * func_80058938 fills a free entry in; func_800528AC walks the table once a
 * frame, lerps `start` towards `end` by elapsed/duration, drops the result
 * into the model slot's field_DC0, redraws through it and then restores
 * everything it touched.
 *
 * bit 0 of `flags` marks the entry live, bit 1 selects the model slot and
 * bits 3..7 carry the part id override. field_0A is the slot's field_E06 as
 * it stood when the request was made; func_800528AC pushes it back through
 * the part records for the duration of the redraw. */
typedef struct {
    u16 flags;             /* 0x00 */
    u8 pad_02[8];          /* 0x02 */
    u16 field_0A;          /* 0x0A */
    u16 elapsed;           /* 0x0C */
    u16 duration;          /* 0x0E */
    ModelTintColor start;  /* 0x10 */
    ModelTintColor end;    /* 0x14 */
} ModelTintRequest;

typedef char ModelSlotHeadEntry_size_must_be_0x8[
    sizeof(ModelSlotHeadEntry) == 0x8 ? 1 : -1
];
typedef char ModelSlotRow_size_must_be_0x76[
    sizeof(ModelSlotRow) == 0x76 ? 1 : -1
];
typedef char ModelSlotRow_max_offset_must_be_0x74[
    MODEL_OFFSET(ModelSlotRow, max) == 0x74 ? 1 : -1
];
typedef char ModelSlotPart_field_18_offset_must_be_0x18[
    MODEL_OFFSET(ModelSlotPart, field_18) == 0x18 ? 1 : -1
];
typedef char ModelSlot_field_2C8_offset_must_be_0x2C8[
    MODEL_OFFSET(ModelSlot, field_2C8) == 0x2C8 ? 1 : -1
];
typedef char ModelSlot_field_750_offset_must_be_0x750[
    MODEL_OFFSET(ModelSlot, field_750) == 0x750 ? 1 : -1
];
typedef char ModelSlot_field_BEC_offset_must_be_0xBEC[
    MODEL_OFFSET(ModelSlot, field_BEC) == 0xBEC ? 1 : -1
];
typedef char ModelSlot_field_DD8_offset_must_be_0xDD8[
    MODEL_OFFSET(ModelSlot, field_DD8) == 0xDD8 ? 1 : -1
];
typedef char ModelSlot_field_E0E_offset_must_be_0xE0E[
    MODEL_OFFSET(ModelSlot, field_E0E) == 0xE0E ? 1 : -1
];
typedef char ModelSlotCF8Block_size_must_be_0x1C[
    sizeof(ModelSlotCF8Block) == 0x1C ? 1 : -1
];
typedef char ModelSlotCF8BlockWords_size_must_match_block[
    sizeof(ModelSlotCF8BlockWords) == sizeof(ModelSlotCF8Block) ? 1 : -1
];
typedef char ModelBytes8_size_must_be_8[
    sizeof(ModelBytes8) == 8 ? 1 : -1
];
typedef char ModelSlotCF8Block_field_0A_offset_must_be_0xA[
    MODEL_OFFSET(ModelSlotCF8Block, field_0A) == 0xA ? 1 : -1
];
typedef char ModelSlotCF8Block_field_0C_offset_must_be_0xC[
    MODEL_OFFSET(ModelSlotCF8Block, field_0C) == 0xC ? 1 : -1
];
typedef char ModelSlotLightEntry_size_must_be_0x10[
    sizeof(ModelSlotLightEntry) == 0x10 ? 1 : -1
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
typedef char ModelSlot_field_1E0_offset_must_be_0x1E0[
    MODEL_OFFSET(ModelSlot, field_1E0) == 0x1E0 ? 1 : -1
];
typedef char ModelSlot_field_750_max_offset_must_be_0x7C4[
    MODEL_OFFSET(ModelSlot, field_750[0].max) == 0x7C4 ? 1 : -1
];
typedef char ModelSlot_field_BF5_offset_must_be_0xBF5[
    MODEL_OFFSET(ModelSlot, field_BF5) == 0xBF5 ? 1 : -1
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
typedef char ModelSlot_field_D18_offset_must_be_0xD18[
    MODEL_OFFSET(ModelSlot, field_D18) == 0xD18 ? 1 : -1
];
typedef char ModelSlot_field_D70_offset_must_be_0xD70[
    MODEL_OFFSET(ModelSlot, field_D70) == 0xD70 ? 1 : -1
];
typedef char ModelSlot_field_DA0_offset_must_be_0xDA0[
    MODEL_OFFSET(ModelSlot, field_DA0) == 0xDA0 ? 1 : -1
];
typedef char ModelSlot_field_DB0_offset_must_be_0xDB0[
    MODEL_OFFSET(ModelSlot, field_DB0) == 0xDB0 ? 1 : -1
];
typedef char ModelSlot_field_DC0_offset_must_be_0xDC0[
    MODEL_OFFSET(ModelSlot, field_DC0) == 0xDC0 ? 1 : -1
];
typedef char ModelSlot_field_DC8_offset_must_be_0xDC8[
    MODEL_OFFSET(ModelSlot, field_DC8) == 0xDC8 ? 1 : -1
];
typedef char ModelSlot_field_DD0_offset_must_be_0xDD0[
    MODEL_OFFSET(ModelSlot, field_DD0) == 0xDD0 ? 1 : -1
];
typedef char ModelSlot_field_DF8_offset_must_be_0xDF8[
    MODEL_OFFSET(ModelSlot, field_DF8) == 0xDF8 ? 1 : -1
];
typedef char ModelSlot_field_DFA_offset_must_be_0xDFA[
    MODEL_OFFSET(ModelSlot, field_DFA) == 0xDFA ? 1 : -1
];
typedef char ModelSlot_field_DFC_offset_must_be_0xDFC[
    MODEL_OFFSET(ModelSlot, field_DFC) == 0xDFC ? 1 : -1
];
typedef char ModelSlot_field_DFE_offset_must_be_0xDFE[
    MODEL_OFFSET(ModelSlot, field_DFE) == 0xDFE ? 1 : -1
];
typedef char ModelSlot_field_DFF_offset_must_be_0xDFF[
    MODEL_OFFSET(ModelSlot, field_DFF) == 0xDFF ? 1 : -1
];
typedef char ModelSlot_field_E06_offset_must_be_0xE06[
    MODEL_OFFSET(ModelSlot, field_E06) == 0xE06 ? 1 : -1
];
typedef char ModelSlot_field_E0D_offset_must_be_0xE0D[
    MODEL_OFFSET(ModelSlot, field_E0D) == 0xE0D ? 1 : -1
];
typedef char ModelSlot_field_E11_offset_must_be_0xE11[
    MODEL_OFFSET(ModelSlot, field_E11) == 0xE11 ? 1 : -1
];
typedef char ModelSlot_field_E12_offset_must_be_0xE12[
    MODEL_OFFSET(ModelSlot, field_E12) == 0xE12 ? 1 : -1
];
typedef char ModelSlot_field_E14_offset_must_be_0xE14[
    MODEL_OFFSET(ModelSlot, field_E14) == 0xE14 ? 1 : -1
];
typedef char ModelSlot_field_E16_offset_must_be_0xE16[
    MODEL_OFFSET(ModelSlot, field_E16) == 0xE16 ? 1 : -1
];
typedef char ModelSlot_entry_count_offset_must_be_0xE17[
    MODEL_OFFSET(ModelSlot, entry_count) == 0xE17 ? 1 : -1
];
typedef char ModelSlot_field_E18_offset_must_be_0xE18[
    MODEL_OFFSET(ModelSlot, field_E18) == 0xE18 ? 1 : -1
];
typedef char ModelSlot_field_E1A_offset_must_be_0xE1A[
    MODEL_OFFSET(ModelSlot, field_E1A) == 0xE1A ? 1 : -1
];
typedef char ModelSlot_field_E1B_offset_must_be_0xE1B[
    MODEL_OFFSET(ModelSlot, field_E1B) == 0xE1B ? 1 : -1
];
typedef char ModelSlot_field_E1F_offset_must_be_0xE1F[
    MODEL_OFFSET(ModelSlot, field_E1F) == 0xE1F ? 1 : -1
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

/* One entry of the eight-byte table at D_80091570.  Every access in the tree
 * is sixteen bits wide: func_8005F5C8 reads field_00, func_8005F27C reads the
 * same halfword through a byte cursor stepping 8, and func_8005A618 reads
 * `angle` and wraps it modulo a full turn.  The retail bytes agree -- the
 * first entries are 02BC / FE00 / FF00 / 0000 and 02BC / 0200 / FF00 / 0000,
 * where field_00 is a constant 700 and the second halfword steps in eighths
 * of a turn.  Nothing reads the record as two 32-bit words.
 *
 * The table is fifteen records: it ends at 0x800915E8, where the next object
 * begins and has none of this pattern.  The declaration is left unsized
 * anyway, because func_8005A618 masks its index with 0x1F and so can reach
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
typedef char ModelTintRequest_size_must_be_0x18[
    sizeof(ModelTintRequest) == 0x18 ? 1 : -1
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
extern ModelEffectCoefficient D_80091570[];
#endif
#ifndef MODEL_SLOT_CUSTOM_EXTERN
extern ModelSlot D_800F2C40[MODEL_SLOT_COUNT];
#endif
#ifndef MODEL_CAMERA_MOVE_CUSTOM_EXTERN
extern ModelCameraMove D_800F2B20;
#endif
#ifndef MODEL_TINT_REQUEST_CUSTOM_EXTERN
extern ModelTintRequest D_800F2B50[MODEL_TINT_REQUEST_COUNT];
#endif
extern ModelHandlerRegistryEntry
    D_800F5918[MODEL_HANDLER_REGISTRY_COUNT];

/* Rebuilds a coordinate unit's local matrix from its Euler angles. Declared
 * on the incomplete type so callers that only forward a slot's field_D18 do
 * not have to pull in the libhmd chain. */
extern void func_8005922C(struct _GsCOORDUNIT *unit, void *scale);

#endif
