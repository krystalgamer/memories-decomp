#ifndef MEMORIES_DECOMP_MODEL_EFFECT_HANDLERS_H
#define MEMORIES_DECOMP_MODEL_EFFECT_HANDLERS_H

#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

typedef struct {
    u8 colors[3][6];
    s16 minimum_height;
    s16 minimum_radius;
    s16 growth_duration;
    s16 fade_duration;
    s16 pad;
} ModelSubdividedEffectConfig;

typedef struct {
    ModelSubdividedEffectConfig *config;
    SVECTOR vertices[6];
    SVECTOR *vertex_links[24];
    SVECTOR subdivided_vertices[384];
    Triplet colors[6];
    u8 *color_links[24];
    Triplet subdivided_colors[384];
    u8 field_130C;
    u8 field_130D;
    u8 pad_130E[2];
    s32 elapsed;
    s32 remaining;
} ModelSubdividedEffect;

typedef char ModelSubdividedEffectConfig_size_must_be_28[
    sizeof(ModelSubdividedEffectConfig) == 28 ? 1 : -1
];
#if !defined(__SIZEOF_POINTER__) || __SIZEOF_POINTER__ == 4
typedef char ModelSubdividedEffect_size_must_be_0x1318[
    sizeof(ModelSubdividedEffect) == 0x1318 ? 1 : -1
];
#endif

extern VECTOR gModel_SubdividedEffectInitialScale;
extern VECTOR gModel_BurstEffectInitialScale;
extern VECTOR gModel_SparkEffectInitialScale;

s32 Model_UpdateSubdividedEffect(ModelSubdividedEffect *data, s32 mode);

s16 *Model_SubdivideTriangle(s16 *a, s16 *b, s16 *c, s16 *out, s32 depth);
Triplet *Triplet_SubdivideTriangle(u8 *a, u8 *b, u8 *c, Triplet *out, s32 depth);
void Triplet_SetComponents(u8 *output, s32 first, s32 second, s32 third);
void Triplet_CopyComponents(u8 *destination, const u8 *source);
void Triplet_AverageComponents(u8 *out, u8 *a, u8 *b);

/* The parameter record Model_UpdateDiscEffect points its `table` at: the rim
   colour, the smallest radius the disc is built with, the level it starts at
   and fades from, and the scale it grows to. The last two are also the
   divisors of the colour ramp. gModel_DiscEffectConfig is the only instance, and its last
   halfword is zero and unread. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad_03;
    s16 min_radius;
    s16 full_level;
    s16 full_scale;
    u16 pad_0A;
} ModelDiscEffectConfig;

/* State for Model_UpdateDiscEffect, one of the four handlers in the pointer
   table at gModel_apfnEffectHandlers. request >= 0 builds a disc of 67 points: a centre at a
   third of the radius and two 33-point rings at the radius and at four fifths
   of it, with the three decimal digits of request turned into the centre colour.
   Negative request draws the disc as 32 Gouraud triangles and advances level
   (-2) or scale (any other value). */
typedef struct {
    ModelDiscEffectConfig *table;
    SVECTOR points[67];
    u8 r;
    u8 g;
    u8 b;
    u8 pad21F;
    s32 unk220;
    s32 level;
    s32 scale;
    u8 frame;
} ModelDiscEffectState;

/* Sixteen bytes of rodata copied whole into the handler's scale vector. */
extern VECTOR gModel_DiscEffectInitialScale;

/* Particle state for Model_UpdateBurstEffect, a handler in the gModel_apfnEffectHandlers model effect
 * table. Three expanding rings, 64 dust particles, 32 sparks with drift and
 * rise vectors, 32 embers and 64 smoke particles, each with its frame and
 * colour. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad;
} ModelBurstColor;

/* The ring colours `table` points at, gModel_BurstRingPaletteWords in model_effect_handlers.c:
 * one byte per ring for each channel. */
typedef struct {
    u8 r[3];
    u8 g[3];
    u8 b[3];
} ModelBurstPalette;

/* One texture record of gModel_BurstImageData in model_effect_handlers.c: the pixel mode
 * and the frame-buffer rectangles of the image and of its colour table. */
typedef struct {
    u16 mode;
    u16 unk_02;
    RECT prect;
    u32 *paddr;
    RECT crect;
    u32 *caddr;
} ModelBurstImage;

/* The texture page and colour table words built from a ModelBurstImage. */
typedef struct {
    u16 tpage;
    u16 clut;
} ModelBurstTexture;

typedef struct {
    ModelBurstPalette *table;
    s32 frame;
    SVECTOR rings[3];
    SVECTOR ring_speed[3];
    SVECTOR dust[64];
    SVECTOR dust_speed[64];
    SVECTOR sparks[32];
    SVECTOR spark_drift[32];
    SVECTOR spark_rise[32];
    SVECTOR embers[32];
    SVECTOR smoke[64];
    SVECTOR smoke_speed[64];
    SVECTOR origin;
    u16 dust_frame[64];
    u16 spark_frame[32];
    u16 smoke_frame[64];
    u16 radius;
    u16 phase;
    u16 stage;
    u16 spark_count;
    u8 fade;
    u8 pad_d89;
    u16 tpage;
    u16 clut;
    ModelBurstTexture spark_texture[1];
    ModelBurstColor colors[3];
    u8 flash_r;
    u8 flash_g;
    u8 flash_b;
    u8 pad_da1;
    ModelBurstColor spark_colors[32];
    u8 smoke_r;
    u8 smoke_g;
    u8 smoke_b;
} ModelBurstEffect;

/* Particle state for Model_UpdateSparkEffect, the fourth handler in gModel_apfnEffectHandlers. It
 * holds 32 sparks with their sizes and colours, 32 flashes and 64 dust
 * particles with their speeds and frames. */
typedef struct {
    u16 size;
    u16 grow;
} ModelSparkSize;

typedef struct {
    u8 r;
    u8 g;
    u8 b;
    u8 pad;
} ModelSparkColor;

typedef struct {
    SVECTOR sparks[32];
    ModelSparkSize sizes[32];
    SVECTOR flashes[32];
    SVECTOR dust[64];
    SVECTOR dust_speed[64];
    u16 dust_frame[64];
    u16 spark_count;
    u16 flash_count;
    u16 frame;
    u16 mode;
    u8 fade;
    u8 pad709;
    u16 tpage;
    u16 clut;
    ModelSparkColor spark_colors[32];
    ModelSparkColor flash_colors[32];
    u8 dust_r;
    u8 dust_g;
    u8 dust_b;
} ModelSparkEffect;

s32 Model_UpdateDiscEffect(ModelDiscEffectState *state, s32 request);
s32 Model_UpdateBurstEffect(void *data, s32 request);
s32 Model_UpdateSparkEffect(void *data, s32 request);

#endif
