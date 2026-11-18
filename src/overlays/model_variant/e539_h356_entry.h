#ifndef FRENCH_MODEL_VARIANT373_ENTRY_H
#define FRENCH_MODEL_VARIANT373_ENTRY_H

#include "../../types.h"
#include "e568_entry_pal.h"
#include "e539_h356_layers.h"
#include "../../game/model_slot_getters.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 count;
    u8 unknown_0E[2];
    u32 rings_start, sphere_start, scale_end, translation_start, translation_end, sparks_start;
} EntryConfig373;

typedef struct {
    u8 unknown[0x3A];
    u16 clut2, clut1, clut0;
} EntryTextureStack373;

typedef struct {
    SVECTOR points[16], corners[4], rotation;
    s32 scale;
    u8 unknown_AC[8];
} EntrySparkGroup373;

typedef struct {
    SVECTOR points[3][2];
    long projected[3][2];
    CVECTOR color;
    VECTOR scale;
    s16 field_5C, field_5E;
    s32 field_60;
    long depth[2];
} EntryStrip373;

typedef union {
    POLY_GT4 sphere[8];
    struct {
        u8 unknown_00[0x34];
        POLY_FT4 pair[2], tiles[8];
    } flat;
} EntryPackets373;

typedef struct {
    ModelVariantDome sphere;
    Family373Ring rising_rings[5];
    EntrySparkGroup373 sparks[3];
    EntryStrip373 strips[1];
    ModelVariantQuadSheet quadrants[1];
    ModelVariantRibbonSpan ribbons[8];
    ModelVariantColorRing rings[4];
    ModelVariantShadedRay rays[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 shared[2], ring_packets[16];
    EntryPackets373 packets;
    POLY_FT4 spark_packet;
    u8 unknown_238C[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    EntryConfig373 *config;
    u32 field_241C;
    GsCOORDUNIT *parts[3];
    s32 index, sphere_scale, translation, rotation, oscillation, sphere_brightness;
    s32 scale, brightness, fields_244C[5];
    s16 ray_polar_angle, ray_azimuth, field_2464, field_2466, ray_radius, field_246A;
    s16 field_246C, field_246E, field_2470, field_2472, factor, width;
    s32 angle, field_247C, texture_offset, phase, field_2488, field_248C;
    u8 unknown_2490[8];
    s32 ambient;
    s16 slot, command;
} EntryState373;

extern GsIMAGE gFrenchModelVariant373_aImages[4];
extern EntryConfig373 gFrenchModelVariant373_aConfigs[];
/* The entry draws the rising rings, the sphere thrown along the aim and the
 * sparks at the target. In an emulator trace (#418, slot-0 attack, two Venus
 * episodes) these three ran and the strip, ribbon, quad and ray helpers below
 * never did; slot 1 is untraced until
 * tools/trace/model_variant373_slot1_helper_reach.lua is run. */
void FrenchModelVariant373_DrawRisingRings(EntryState373 *context);
void FrenchModelVariant373_DrawSphere(EntryState373 *context);
void FrenchModelVariant373_DrawSparks(EntryState373 *context);
void FrenchModelVariant373_DrawStrip(EntryState373 *context);
void FrenchModelVariant373_DrawRibbons(EntryState373 *context);
void FrenchModelVariant373_DrawQuads(EntryState373 *context);
void FrenchModelVariant373_DrawRays(EntryState373 *context);

#endif
