#ifndef YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H
#define YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H

#include "../types.h"

/* Layout-compatible view of libhmd's GsTYPEUNIT. Kept local so users that
 * only need registry declarations do not inherit libhmd's include chain. */
typedef struct {
    u32 type;
    u32 *ptr;
} ModelTypeUnit;

/* Pointer-bearing prefix used by the type-2 unit processor's scratch block. */
typedef struct {
    u8 *records;
    u8 pad_04[0x14];
    u8 *indices;
} ModelType2Scratch;

typedef char ModelType2Scratch_indices_offset_must_be_0x18[
    ((u32)&(((ModelType2Scratch *)0)->indices)) == 0x18 ? 1 : -1
];

/* Four-byte prefix of the type-2 record stream. */
typedef struct {
    u16 field_00;
    u16 record_count;
} ModelType2RecordHeader;

typedef char ModelType2RecordHeader_size_must_be_4[
    sizeof(ModelType2RecordHeader) == 4 ? 1 : -1
];

/* Twelve-byte stream record. Only fields with established loop/index roles
 * are named. */
typedef struct {
    u8 pad_00[6];
    s16 row_count;
    s32 index_offset;
} ModelType2Record;

typedef char ModelType2Record_size_must_be_0xC[
    sizeof(ModelType2Record) == 0xC ? 1 : -1
];

void Model_RegisterHandlerKey(s32 key, s32 val);
s32 Model_FindHandlerKey(s32 val);
void Model_ProcessType2Unit(
    s32 model_index, ModelTypeUnit *unit, u8 *scratch);
void func_80069E44(void);
void func_80069F94(void);
void func_8006A0E8(void);
void func_8006A268(void);
void func_8006A3F0(void);
void func_8006A540(void);
void func_8006A694(void);
void func_8006A814(void);
void func_8006A99C(void);
void func_8006AAFC(void);
void func_8006AC88(void);
void func_8006ADE8(void);

/* Open-codes the same reverse lookup as Model_FindHandlerKey -- same
 * sentinel, same eighty-entry scan of D_800F5918, same -1 -- and dispatches on
 * the packed id it returns. func_80059DD8 in model_scene_states.c is the only
 * outside consumer and takes its address as an s32 (*)(s32) callback rather
 * than calling it, so this declaration has to keep that exact shape. */
s32 func_8005FC1C(s32 arg0);

#endif
