#ifndef FRENCH_MODEL_VARIANT478_ENTRY_H
#define FRENCH_MODEL_VARIANT478_ENTRY_H

#include "../../types.h"
#include "../french_model_variant/variant402_entry.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_camera_effects.h"

typedef struct {
    CVECTOR inner, outer, grid;
    u8 unknown_0C[4], parts[3], unknown_13[9];
    u32 start, swarm_start, swarm_end, ring_end, fade_start;
} EntryConfig478;

typedef struct {
    SVECTOR a[32], b[32], c[32], d[32], positions[32];
    CVECTOR color;
    u8 unknown_504[0x10];
    s32 size[32], done[32], reset[32];
} EntrySwarm478;


typedef struct {
    SVECTOR v0[8], v1[8], v2[8], v3[8], positions[8];
    CVECTOR color;
    s32 size[8], field_164[8], hidden[8], field_1A4[8], field_1C4[8];
    u8 unknown_1E4[0x80];
} EntryPetals478;

typedef struct {
    ModelVariantQuadGroup quads[1];
    EntryPetals478 petals[3];
    EntrySwarm478 swarm;
    ModelVariantTrail extras[16];
    ModelVariantWebNarrow grids[3];
    ModelVariantBandPair pairs[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6], large_rings[4];
    ModelVariantShape small[1];
    ModelVariantPackets packets;
    u8 unknown_28C4[0x34];
    POLY_FT4 flat[2], swarm_packet;
    u8 unknown_2970[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    EntryConfig478 *config;
    s32 field_2A00;
    GsCOORDUNIT *parts[3];
    s16 field_2A10, field_2A12, field_2A14, field_2A16;
    s32 field_2A18, wave, field_2A20, phase;
    SVECTOR camera_offset;
    s32 ambient;
    s16 slot, command;
} EntryState478;

extern GsIMAGE gFrenchModelVariant478_aImages[9];
extern EntryConfig478 gFrenchModelVariant478_aConfigs[];
void FrenchModelVariant478_DrawSwarm(EntryState478 *context);
void SpanishModelVariant478_DrawOrbitingQuads(EntryState478 *context);
void SpanishModelVariant478_DrawPetalRings(EntryState478 *context);

#endif
