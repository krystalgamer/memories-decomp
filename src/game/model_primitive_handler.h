#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* Layout-compatible view of libhmd's GsTYPEUNIT. */
typedef struct {
    u32 type;
    u32 *ptr;
} ModelTypeUnit;

/* Image-context prefix; +0x18 is the SDK CLUT-data pointer. */
typedef struct {
    u8 *records;
    u8 pad_04[0x14];
    u8 *clut_data;
} ModelType2Scratch;

typedef char ModelType2Scratch_clut_data_offset_must_be_0x18[
    OFFSETOF(ModelType2Scratch, clut_data) == 0x18 ? 1 : -1
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

void Model_ProcessType2Unit(
    s32 model_index, ModelTypeUnit *unit, u8 *scratch);

/* Returns the libhmd image-primitive driver for an IMAGE primitive type. */
u32 *(*Model_GetImagePrimitiveHandler(u32 value))(GsARGUNIT *);
void *Model_GetPrimitiveHandler(u32 primitive_code);
void Model_BindPrimitiveHandler(ModelHandlerObject *object);
s32 Model_GetSharedPrimitiveHandler(s32 primitive_code);
void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object);

#endif
