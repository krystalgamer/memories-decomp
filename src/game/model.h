#ifndef YUGIOH_GAME_MODEL_H
#define YUGIOH_GAME_MODEL_H

#include "../types.h"

#define MODEL_OFFSET(type, member) ((u32)&(((type *)0)->member))

#define MODEL_SLOT_COUNT 3
#define MODEL_SLOT_SIZE 0xE20
#define MODEL_SLOT_DATA_ENTRY_SIZE 80
#define MODEL_HANDLER_REGISTRY_COUNT 80
#define MODEL_DATA_MIN_FREE_BYTES 0x401
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

typedef struct {
    u16 field_00;
    u8 pad_02[0x74];
} ModelSlotIndexedEntry;

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
    u8 pad_00[0x44];
    u16 field_44;
    u16 field_46;
    u16 field_48;
} ModelSlotRotationEntry;

typedef struct {
    ModelSlotHeadEntry field_000[1];
    u8 pad_008[0x1D8];
    void *field_1E0[1];
    u8 pad_1E4[0x5E0];
    ModelSlotIndexedEntry field_7C4[1];
    u8 pad_83A[0x3BB];
    u8 field_BF5;
    u8 pad_BF6[0x102];
    u8 field_CF8[10];
    u8 pad_D02[0x12];
    u8 *field_D14;
    ModelSlotRotationEntry *field_D18;
    u8 pad_D1C[0x54];
    ModelSlotLightEntry field_D70[3];
    s32 field_DA0[3];
    u8 pad_DAC[4];
    ModelSlotS32Quad field_DB0;
    u8 field_DC0[8];
    u16 field_DC8[4];
    u16 field_DD0[4];
    u8 pad_DD8[0x2E];
    u16 field_E06;
    u8 pad_E08[5];
    u8 field_E0D;
    u8 pad_E0E[3];
    u8 field_E11;
    u8 field_E12;
    u8 field_E13;
    u8 field_E14;
    u8 field_E15;
    u8 field_E16;
    u8 field_E17;
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

typedef char ModelSlotHeadEntry_size_must_be_0x8[
    sizeof(ModelSlotHeadEntry) == 0x8 ? 1 : -1
];
typedef char ModelSlotIndexedEntry_size_must_be_0x76[
    sizeof(ModelSlotIndexedEntry) == 0x76 ? 1 : -1
];
typedef char ModelSlotLightEntry_size_must_be_0x10[
    sizeof(ModelSlotLightEntry) == 0x10 ? 1 : -1
];
typedef char ModelSlotS32Quad_size_must_be_0x10[
    sizeof(ModelSlotS32Quad) == 0x10 ? 1 : -1
];
typedef char ModelSlotRotationEntry_field_44_offset_must_be_0x44[
    MODEL_OFFSET(ModelSlotRotationEntry, field_44) == 0x44 ? 1 : -1
];
typedef char ModelSlot_size_must_be_0xE20[
    sizeof(ModelSlot) == MODEL_SLOT_SIZE ? 1 : -1
];
typedef char ModelSlot_field_1E0_offset_must_be_0x1E0[
    MODEL_OFFSET(ModelSlot, field_1E0) == 0x1E0 ? 1 : -1
];
typedef char ModelSlot_field_7C4_offset_must_be_0x7C4[
    MODEL_OFFSET(ModelSlot, field_7C4) == 0x7C4 ? 1 : -1
];
typedef char ModelSlot_field_BF5_offset_must_be_0xBF5[
    MODEL_OFFSET(ModelSlot, field_BF5) == 0xBF5 ? 1 : -1
];
typedef char ModelSlot_field_CF8_offset_must_be_0xCF8[
    MODEL_OFFSET(ModelSlot, field_CF8) == 0xCF8 ? 1 : -1
];
typedef char ModelSlot_field_D14_offset_must_be_0xD14[
    MODEL_OFFSET(ModelSlot, field_D14) == 0xD14 ? 1 : -1
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
typedef char ModelSlot_field_E17_offset_must_be_0xE17[
    MODEL_OFFSET(ModelSlot, field_E17) == 0xE17 ? 1 : -1
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

#undef MODEL_OFFSET

#ifndef MODEL_SLOT_CUSTOM_EXTERN
extern ModelSlot D_800F2C40[MODEL_SLOT_COUNT];
#endif
extern ModelHandlerRegistryEntry
    D_800F5918[MODEL_HANDLER_REGISTRY_COUNT];

#endif
