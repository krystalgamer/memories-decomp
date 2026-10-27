#ifndef YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H
#define YUGIOH_GAME_MODEL_HANDLER_REGISTRY_H

#include "../types.h"

/* Layout-compatible view of libhmd's GsTYPEUNIT. Kept local so users that
 * only need registry declarations do not inherit libhmd's include chain. */
typedef struct {
    u32 type;
    u32 *G32 ptr;
} ModelTypeUnit;

/* Image-context prefix; +0x18 is the SDK CLUT-data pointer. */
typedef struct {
    u8 *G32 records;
    u8 pad_04[0x14];
    u8 *G32 clut_data;
} ModelType2Scratch;

typedef char ModelType2Scratch_clut_data_offset_must_be_0x18[
    ((u32)&(((ModelType2Scratch *)0)->clut_data)) == 0x18 ? 1 : -1
];

/* Four-byte prefix of the type-2 record stream; its size advances the stream
 * cursor in Model_ProcessType2Unit. */
typedef struct {
    u16 descriptor_word_count;
    u16 record_count;
} ModelType2RecordHeader;

typedef char ModelType2RecordHeader_size_must_be_4[
    sizeof(ModelType2RecordHeader) == 4 ? 1 : -1
];

/* Twelve-byte image/CLUT record. Coordinate types preserve the original
 * view rather than substituting RECT; sizeof drives both record advances. */
typedef struct {
    u16 vram_x;
    s16 vram_y;
    s16 column_count;
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
 * sentinel, same eighty-entry scan of gModel_aHandlerKeyRegistry, same -1 -- and dispatches on
 * the packed id it returns. Model_RunSlotHandlers in model_scene_states.c is
 * the only outside consumer and takes its address as an s32 (*)(s32) callback
 * rather than calling it, so this declaration has to keep that exact shape. */
s32 func_8005FC1C(s32 arg0);

/* The same case one function along, and for the same reason. Model_DrawSlot
 * (src/game/model_draw_slot.c) is the only outside consumer and takes its
 * address rather than calling it, handing it to Model_RemapPrimitiveHandlers, whose first
 * parameter is declared `s32 (*fn)(s32)` (model_remap_primitive_handlers.h). So this
 * declaration has to keep that exact shape too; it is the definition's own
 * spelling, unchanged. */
s32 func_8005FE44(s32 arg0);

#endif
