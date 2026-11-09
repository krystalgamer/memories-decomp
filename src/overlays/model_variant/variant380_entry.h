#ifndef MEMORIES_MODEL_VARIANT380_ENTRY_H
#define MEMORIES_MODEL_VARIANT380_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../french_model_variant/variant337_entry.h"

/* One 0x44-byte command row in gModelVariant380_aConfigs. */
typedef struct {
    u8 unknown_00[8];
    s32 parts[5];
    u8 unknown_1C[8];
    s32 count;
    s32 divisor;
    u32 start, end;
    u32 fade_at;
    u8 unknown_38[0xC];
} Variant380EntryConfig;

/* Up to five 0x2D4-byte beams at the start of the work area. */
typedef struct {
    u8 unknown_000[0x1A0];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_1B8, field_1BA, field_1BC;
    u8 unknown_1BE[0x228 - 0x1BE];
    VECTOR velocity;
    u8 unknown_238[0x2D4 - 0x238];
} Variant380EntryBeam;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant380EntryBeam beams[5];
    ModelVariantSheetSet sheets[5];
    Variant337EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[6];
    u8 unknown_19B8[0x10];
    MATRIX matrices[5];
    SVECTOR target;
    VECTOR targets[5];
    VECTOR directions[5];
    s16 screen_dx[5], screen_dy[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, span, fade;
    Variant380EntryConfig *config;
    u32 field_1B58;
    GsCOORDUNIT *parts[5];
    s16 length[5], field_1B7A[5], mode[5], done[5];
    s16 field_1B98, field_1B9A, width;
    s32 streamer_length[5], streamer_rotation_x[5], streamer_rotation_y[5], streamer_rotation_z[5];
    s32 wave, bend, phase;
    u8 unknown_1BFC[8];
    s32 ambient;
    s16 slot, command;
} Variant380EntryState;

extern GsIMAGE gModelVariant380_aImages[2];
extern Variant380EntryConfig gModelVariant380_aConfigs[];
void FrenchModelVariant397_DrawBeams(SVECTOR *point);
void FrenchModelVariant397_DrawSheets(SVECTOR *point);
void FrenchModelVariant397_DrawStreamers(SVECTOR *point);
s32 ModelVariant380_HandleRequest(SVECTOR *point, s32 command);

#endif
