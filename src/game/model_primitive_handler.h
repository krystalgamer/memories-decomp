#ifndef YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H
#define YUGIOH_GAME_MODEL_PRIMITIVE_HANDLER_H

#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"

/* Four-byte prefix of the type-2 record stream. */
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

/* The type-2 record stream GsARGUNIT_IMAGE.primp points at: the header,
 * then each image record, followed by its CLUT record for image-with-CLUT
 * units. */
typedef struct {
    ModelType2RecordHeader header;
    ModelType2Record records[1];
} ModelType2RecordStream;

void Model_ProcessType2Unit(
    s32 model_index, GsTYPEUNIT *unit, GsARGUNIT *scratch);

/* Returns the libhmd image-primitive driver for an IMAGE primitive type. */
u32 *(*Model_GetImagePrimitiveHandler(u32 value))(GsARGUNIT *);
void *Model_GetPrimitiveHandler(u32 primitive_code);
void Model_BindPrimitiveHandler(ModelHandlerObject *object);
s32 Model_GetSharedPrimitiveHandler(s32 primitive_code);
void Model_BindSharedPrimitiveHandler(ModelHandlerObject *object);

#endif
