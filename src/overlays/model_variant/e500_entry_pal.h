#ifndef MEMORIES_FRENCH_MODEL_VARIANT338_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT338_ENTRY_H
#include "../../types.h"
#include "e503_h320_entry_pal.h"
#include "e500_ribbon.h"

typedef struct {
    CVECTOR ribbon, inner, outer;
    u8 part, unknown_0D[5];
    u16 vertex;
    u8 unknown_14[8];
    s32 mode;
    u32 start;
    u32 end;
    u8 unknown_28[4];
    u32 fade_start, fade_end;
} Variant338EntryConfig;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    ModelVariantSpineRibbon records[5];
    ModelVariantPointSheet rings[2];
    ModelVariantStrandWide strands[6];
    ModelVariantFlaggedStreamer streamers[2];
    ModelVariantPackets packets;
    POLY_FT4 flat_textured[2];
    POLY_FT4 extra_textured[2];
    GsLINE line;
    MATRIX matrix;
    VECTOR vertex_position;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant338EntryConfig *config;
    u32 field_1F40;
    GsCOORDUNIT *part;
    s16 field_1F48, strand_start, strand_end, width;
    s32 streamer_length, streamer_rotation_x, streamer_rotation_y, streamer_rotation_z;
    s32 ribbon_wave, ribbon_phase, phase;
    u8 unknown_1F6C[8];
    s32 ambient;
    s16 slot, command;
} Variant338EntryState;

extern GsIMAGE gFrenchModelVariant338_aImages[9];
extern Variant338EntryConfig gFrenchModelVariant338_aConfigs[];
void ModelVariant321_DrawRibbon(Variant338EntryState *context);
void ModelVariant321_DrawRings(Variant338EntryState *context);
void FrenchModelVariant338_DrawStreamers(Variant338EntryState *context);
s32 FrenchModelVariant338_HandleRequest(Variant338EntryState *context, s32 command);
#endif
