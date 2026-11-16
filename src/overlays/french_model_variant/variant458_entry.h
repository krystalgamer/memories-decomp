#ifndef FRENCH_MODEL_VARIANT458_ENTRY_H
#define FRENCH_MODEL_VARIANT458_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4], parts[7], unknown_0B[5];
    s32 draw_start, sheet_start, sheet_end;
    s32 move_start, move_end, fade_start;
} EntryConfig458;

typedef union {
    DVECTOR point;
    long packed;
} Variant458Screen;

typedef struct {
    SVECTOR points[9];
    Variant458Screen screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    Variant458Screen edge_screen[9];
    s32 width[9];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0x68];
    s32 depth[9];
    s16 x_offset[9];
    s16 y_offset[9];
    s32 delay, count, field_1F8, field_1FC;
} EntryStrand458;

typedef struct {
    EntryStrand458 strands[6];
    ModelVariantQuadSheet sheets[7];
    ModelVariantBandTriple ribbons[1];
    ModelVariantShortStreamer streamers[3];
    u8 unknown_17C8[0x318];
    ModelVariantPackets packets;
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
    s32 frame_count;
    s32 elapsed;
    u32 animation;
    s32 step, duration, fade;
    EntryConfig458 *config;
    u32 field_1F28;
    GsCOORDUNIT *parts[7];
    s16 field_1F48, field_1F4A;
    s32 field_1F4C, field_1F50, field_1F54, field_1F58, spin;
    s16 field_1F60, field_1F62;
    ModelVariantStreamerParams streamer;
    s32 strand_phase, wave_phase;
    s32 phase;
    u8 unknown_1F80[8];
    s32 ambient;
    s16 slot, command;
} EntryState458;

extern u8 gFrenchModelVariant458_D230C[];
void FrenchModelVariant458_DrawSheets(EntryState458 *context);
void FrenchModelVariant458_DrawStrands(EntryState458 *context);
void FrenchModelVariant458_DrawStreamers(EntryState458 *context);

#endif
