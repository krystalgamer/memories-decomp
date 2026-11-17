#ifndef MEMORIES_MODEL_VARIANT380_ENTRY_H
#define MEMORIES_MODEL_VARIANT380_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/variant337_entry.h"

/* One 0x44-byte command row in gModelVariant380_aConfigs. */
typedef struct {
    u8 unknown_00[8];
    s32 parts[5];
    u8 unknown_1C[8];
    s32 count;
    s32 divisor;
    u32 start, end;
    u32 fade_at;
    u32 hide_at, release_at;
    u8 unknown_40[4];
} Variant380EntryConfig;

/* Up to five 0x2D4-byte beams at the start of the work area: a thirteen-point
 * spine and its view-shifted copy, both projected, with per-point angle,
 * width, projection flag, depth and screen offset. */
typedef struct {
    SVECTOR a[13];
    long sa[13];
    s32 angle[13];
    SVECTOR b[13];
    long sb[13];
    s32 width[13];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_1B8, field_1BA, field_1BC;
    u8 unknown_1BE[0x228 - 0x1BE];
    VECTOR velocity;
    long flag[13];
    s32 otz[13];
    s16 ox[13];
    s16 oy[13];
} Variant380EntryBeam;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    Variant380EntryBeam beams[5];
    ModelVariantSheetSet sheets[5];
    ModelVariantFlaggedStreamer streamers[2];
    ModelVariantPackets packets;
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
void FrenchModelVariant397_DrawBeams(Variant380EntryState *context);
void FrenchModelVariant397_DrawSheets(Variant380EntryState *context);
void FrenchModelVariant397_DrawStreamers(Variant380EntryState *context);
s32 ModelVariant380_HandleRequest(Variant380EntryState *context, s32 command);

#endif
