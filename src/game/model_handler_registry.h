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

#endif
