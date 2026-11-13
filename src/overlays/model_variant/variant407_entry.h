#ifndef MEMORIES_MODEL_VARIANT407_ENTRY_H
#define MEMORIES_MODEL_VARIANT407_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    CVECTOR inner, outer, web;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u16 petal_size, petal_radius;
    u16 field_20;
    u8 unknown_22[2];
    u32 start;
    u32 field_28;
    u8 unknown_2C[8];
    u32 track_end, sweep_start, sweep_end, sweep_angle;
} Variant407EntryConfig;

typedef struct {
    SVECTOR v0[48], v1[48], v2[48], v3[48], origin[48];
    CVECTOR color;
    u8 unknown_784[0x10];
    s32 scale[48];
    s32 field_854[48];
    s32 field_914[48];
    s32 done[48];
} Variant407EntryPetals;

typedef struct {
    Variant407EntryPetals petals;
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    /* The entry fills a POLY_FT4 template here; the petal drawer reads the
     * same bytes as a POLY_G4. */
    union {
        POLY_FT4 ft4;
        POLY_G4 g4;
    } sprite;
    u8 unknown_2348[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    VECTOR positions[48];
    VECTOR offsets[48];
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant407EntryConfig *config;
    u32 field_29D8;
    GsCOORDUNIT *parts[3];
    s16 field_29E8, field_29EA;
    s32 sweep;
    s16 field_29F0, field_29F2;
    s32 field_29F4, field_29F8;
    ModelVariantEntryTail tail;
} Variant407EntryState;

extern GsIMAGE gModelVariant407_aImages[9];
extern Variant407EntryConfig gModelVariant407_aConfigs[];
void ModelVariant407_DrawPetals(u8 *context);
#endif
