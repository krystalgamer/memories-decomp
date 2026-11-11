#ifndef MEMORIES_MODEL_VARIANT454_ENTRY_H
#define MEMORIES_MODEL_VARIANT454_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"
#include "../../game/model_camera_effects.h"
#include "variant412_entry.h"

/* One group of 16 sprites at the start of the work area. */
typedef struct {
    SVECTOR v0[16], v1[16], v2[16], v3[16], origin[16];
    CVECTOR color;
    u8 unknown_284[0x10];
    s32 scale[16];
    s32 field_2D4[16];
    s32 field_314[16];
    s32 done[16];
    MATRIX origins[16];
    VECTOR positions[16];
    VECTOR offsets[16];
} Variant454EntryGroup;

typedef struct {
    Variant454EntryGroup groups[1];
    ModelVariantSpriteSet clusters[1];
    ModelVariantTrail records[16];
    ModelVariantWebNarrow webs[3];
    ModelVariantBandPair bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantRing rings[6];
    ModelVariantRing spokes[4];
    ModelVariantFan quads[1];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra;
    POLY_FT4 flat_textured[2], sprite, glow;
    u8 unknown_236C[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantSweepConfig *config;
    u32 field_23FC;
    GsCOORDUNIT *parts[3];
    s16 field_240C, field_240E;
    s32 sweep;
    s16 field_2414, field_2416;
    s32 field_2418, field_241C;
    SVECTOR orbit;
    s32 tint;
    s32 phase;
    s16 slot, command;
} Variant454EntryState;

extern GsIMAGE gModelVariant454_aImages[9];
extern ModelVariantSweepConfig gModelVariant454_aConfigs[];
void FrenchModelVariant471_DrawQuadGroup(u8 *context);
void FrenchModelVariant471_DrawQuads(u8 *context);
#endif
