#ifndef FRENCH_MODEL_VARIANT458_ENTRY_H
#define FRENCH_MODEL_VARIANT458_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "variant458_sheets.h"
#include "variant458_strands.h"
#include "variant458_streamers.h"

typedef struct {
    u8 unknown_00[4], parts[7], unknown_0B[5];
    s32 draw_start, sheet_start, sheet_end;
    u8 unknown_1C[0xC];
} EntryConfig458;

typedef struct {
    u8 unknown_000[0x120];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0xB0];
    s32 delay, count, field_1F8, field_1FC;
} EntryStrand458;

typedef struct {
    u8 unknown_00[0x6C];
    s32 offsets[3];
    CVECTOR inner[3], outer[3];
    VECTOR scale;
    s16 field_A0, field_A2;
    s32 field_A4;
    u8 unknown_A8[0xC];
} EntryRibbon458;

typedef struct {
    u8 unknown_000[0x1D4];
    CVECTOR color[13];
    s32 field_208;
    u8 unknown_20C[0x68];
} EntryStreamer458;

typedef struct {
    DVECTOR projected;
    PSXLONG interpolation, flag;
    DVECTOR target;
} EntryProjection458;

typedef struct {
    EntryStrand458 strands[6];
    Variant458Sheet sheets[7];
    EntryRibbon458 ribbons[1];
    EntryStreamer458 streamers[3];
    u8 unknown_17C8[0x318];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2], strand_flat[2];
    u8 unknown_1C28[0x10];
    MATRIX matrix;
    SVECTOR target;
    MATRIX origins[7];
    VECTOR destinations[7];
    u8 unknown_1DB0[0x50];
    s32 factor, field_1E04, scale, field_1E0C;
    u8 unknown_1E10[0x10];
    VECTOR deltas[7], spans[6];
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    s32 elapsed;
    u32 animation;
    s32 step, duration, fade;
    EntryConfig458 *G32 config;
    u32 field_1F28;
    GsCOORDUNIT *G32 parts[7];
    s16 field_1F48, field_1F4A;
    s32 field_1F4C, field_1F50, field_1F54, field_1F58, field_1F5C;
    s16 field_1F60, field_1F62;
    s32 field_1F64, field_1F68, field_1F6C, field_1F70, field_1F74, field_1F78;
    s32 phase;
    u8 unknown_1F80[8];
    s32 ambient;
    s16 slot, command;
} EntryState458;

extern u8 gFrenchModelVariant458_D230C[];
void FrenchModelVariant458_DrawSheets(u8 *context);
void FrenchModelVariant458_DrawStreamers(u8 *ctx);

#endif
