#ifndef MEMORIES_FRENCH_MODEL_VARIANT422_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT422_ENTRY_H
#include "../../types.h"
#include "h320_entry_pal.h"
#include "../model_variant/e588_halo.h"
#include "../model_variant/e588_veils.h"

/* One 0x1C8-byte band: three rows of nine points and their screen
 * coordinates, the two colour rows and scale the entry seeds, and the nine
 * depths. */
typedef struct {
    SVECTOR a[9], b[9], c[9];
    long sa[9], sb[9], sc[9];
    CVECTOR ca[9], cb[9];
    VECTOR scale;
    s32 field_19C;
    s16 field_1A0, field_1A2;
    s32 otz[9];
} Variant422EntryBand;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant405Veil veils[5];
    Variant405Halo halo;
    ModelVariantWebNarrow webs[3];
    Variant422EntryBand bands[1];
    ModelVariantSheet sheets[2];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    ModelVariantFan quads[1];
    POLY_GT4 halo_poly;
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1D3C[0x10];
    GsGLINE line;
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    ModelVariantWebConfig *config;
    u32 field_1DCC;
    GsCOORDUNIT *parts[3];
    u8 unknown_1DDC[0x14];
    s32 field_1DF0, field_1DF4, field_1DF8, field_1DFC;
    s32 field_1E00, field_1E04, field_1E08;
    s16 field_1E0C, field_1E0E, band_scale, field_1E12;
    s32 displacement, quad_angle, phase, field_1E20, field_1E24;
    u8 unknown_1E28[8];
    s32 tint;
    s16 slot, command;
} Variant422EntryState;

extern GsIMAGE gFrenchModelVariant422_aImages[2];
extern ModelVariantWebConfig gFrenchModelVariant422_aConfigs[];
void ModelVariant405_DrawHalo(Variant422EntryState *context);
void ModelVariant405_DrawVeils(Variant422EntryState *context);
#endif
