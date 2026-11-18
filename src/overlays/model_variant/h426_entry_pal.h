#ifndef FRENCH_MODEL_VARIANT443_ENTRY_H
#define FRENCH_MODEL_VARIANT443_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"

typedef struct {
    u8 inner[3][3], unknown_09[3];
    u8 outer[3][3], unknown_15[3];
    CVECTOR sphere_color;
    u8 unknown_1C[4], parts[4];
    u16 vertices[4];
    u8 unknown_2C[12];
    u32 start, web_end, spread_start, spread_end;
    u32 mesh_start, mesh_end;
    u32 fade_start, fade_end;
} EntryConfig443;

typedef struct {
    SVECTOR points[3][4];
    CVECTOR inner, outer;
    s32 progress, hidden;
} EntryFramebufferRing443;

/* One short band: three point pairs and their projections, inner and outer
 * colours, the scale, and per-quad depth. */
typedef struct {
    SVECTOR a[2], b[2], c[2];
    long sa[2], sb[2], sc[2];
    CVECTOR inner[2], outer[2];
    VECTOR scale;
    s16 field_68, field_6A;
    s32 field_6C;
    s32 otz[2];
} EntryShortBand443;

typedef struct {
    ModelVariantMovingRibbon ribbons[3];
    EntryFramebufferRing443 rings[5];
    ModelVariantTube meshes[1];
    ModelVariantShortStreamer streamers[3];
    ModelVariantWebNarrow spheres[3];
    ModelVariantShadedRay spirals[12];
    u8 unknown_23D8[0x360];
    ModelVariantColorBand long_bands[1];
    EntryShortBand443 bands[3];
    ModelVariantSheet sheets[5];
    ModelVariantColorRing rings_a[6], rings_b[4];
    ModelVariantStarShape shapes[1];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat[2], additional[2];
    u8 unknown_350C[0x10];
    GsGLINE line;
    u8 unknown_3530[0x14];
    s32 stored_target[3];
    VECTOR vertices[2], positions[3], origin;
    SVECTOR target;
    VECTOR direction, paths[3];
    DVECTOR projected;
    s16 screen_x[3], screen_y[3];
    VECTOR view_delta;
    ModelVariantClock clock;
    EntryConfig443 *config;
    u8 unknown_3638[0x14];
    s16 field_364C, field_364E;
    s32 factor, progress, mesh_angle, spin;
    s16 field_3660, field_3662, field_3664, field_3666;
    s32 ribbon_start, field_366C, ribbon_wave, ribbon_phase;
    s16 size, field_367A;
    s32 travel, length, rotation_x, rotation_y, rotation_z;
    s32 field_3690, field_3694, phase, field_369C;
    u8 unknown_36A0[8];
    s32 ambient;
    s16 slot, command;
} EntryState443;

extern u8 gFrenchModelVariant443_D45A8[];
void FrenchModelVariant443_DrawRibbons(EntryState443 *context);
void SpanishModelVariant443_DrawRings(EntryState443 *context);
void SpanishModelVariant443_DrawMesh(EntryState443 *context);
void SpanishModelVariant443_DrawSheets(EntryState443 *context);
void FrenchModelVariant443_DrawStreamers(EntryState443 *context);
void FrenchModelVariant443_DrawBands(EntryState443 *context);

#endif
