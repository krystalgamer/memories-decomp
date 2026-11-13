#ifndef FRENCH_MODEL_VARIANT446_ENTRY_H
#define FRENCH_MODEL_VARIANT446_ENTRY_H

#include "../../types.h"
#include "variant402_entry.h"
#include "../../game/model_slot_getters.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u16 count;
    u8 unknown_0E[2];
    s32 width, inner_radius, outer_radius;
    u8 unknown_01C[0x8];
    u16 ribbon_radius;
    u8 unknown_026[0x2];
    s32 extent;
    u32 draw_start, duration, expanded, fade, end, rays_start, rays_end;
    u32 unknown_48;
} EntryConfig446;

typedef struct {
    SVECTOR points[2][4][6];
    CVECTOR color;
    VECTOR direction;
    s32 size, finished, field_19C;
} EntryLines446;

typedef struct {
    SVECTOR points[3][5];
    long projected[3][5];
    s16 field_B4, field_B6;
    s32 field_B8, depth[5];
} EntryStrip446;

typedef struct {
    SVECTOR points[4][4];
    VECTOR scale;
} EntryPulse446;

typedef struct {
    ModelVariantRay rays[16];
    EntryLines446 lines[3];
    EntryStrip446 strips[1];
    EntryPulse446 pulses[2];
    Family402Ribbon ribbons[8];
    ModelVariantCurtain rings[4];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2], extra, ring_packet;
    POLY_FT4 flat[2];
    u8 unknown_1AD0[0x38];
    GsGLINE line;
    MATRIX matrix, second_matrix;
    ModelVariantAim aim;
    s32 angles[2];
    u32 frame_count, elapsed, animation, step;
    s32 fade;
    EntryConfig446 *config;
    u32 unknown_1BA8;
    GsCOORDUNIT *parts[3];
    s32 part_index;
    CVECTOR inner, outer;
    s16 spin, field_1BC6, field_1BC8, field_1BCA;
    s16 width, radius, progress, ring_angle;
    s32 ray_size, ray_length, phase, field_1BE0;
    s16 slot, command;
} EntryState446;

extern GsIMAGE gFrenchModelVariant446_aImages[9];
extern EntryConfig446 gFrenchModelVariant446_aConfigs[];
void SpanishModelVariant446_DrawLines(u8 *context);
void SpanishModelVariant446_DrawStrip(u8 *context);
void FrenchModelVariant446_DrawRibbons(u8 *context);
void SpanishModelVariant446_DrawPulse(u8 *context);
void SpanishModelVariant446_DrawRings(u8 *context);
void FrenchModelVariant446_DrawRays(u8 *context);

#endif
