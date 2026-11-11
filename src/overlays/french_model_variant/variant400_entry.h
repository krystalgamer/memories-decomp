#ifndef MEMORIES_FRENCH_MODEL_VARIANT400_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT400_ENTRY_H
#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4];
    u8 part, unknown_05[7];
    u32 start, grow_end, end, field_18, field_1C;
} Variant400EntryConfig;

typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x2C8 - 0x240];
    VECTOR position;
    u8 unknown_2D8[0x370 - 0x2D8];
} Variant400EntryNode;

typedef struct {
    Variant400EntryNode nodes[5];
    ModelVariantPointSheet sheets[7];
    ModelVariantStreamer columns[2];
    u8 unknown_1BC0[0x1ED8 - 0x1BC0];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat_textured[2];
    u8 unknown_1FD0[0x10];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    u8 unknown_2018[0x10];
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, duration;
    u32 fade;
    Variant400EntryConfig *config;
    u32 field_2060;
    GsCOORDUNIT *part;
    s32 width, segment_start, segment_end;
    s16 field_2074, field_2076;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    s32 ribbon_wave, ribbon_phase;
    ModelVariantEntryTail tail;
} Variant400EntryState;

extern GsIMAGE gFrenchModelVariant400_aImages[9];
extern Variant400EntryConfig gFrenchModelVariant400_aConfigs[];
void FrenchModelVariant400_DrawRibbons(u8 *context);
void FrenchModelVariant400_DrawSheets(u8 *context);
void FrenchModelVariant400_DrawStreamers(u8 *context);
s32 FrenchModelVariant400_HandleRequest(SVECTOR *point, s32 command);
#endif
