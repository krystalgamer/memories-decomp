#ifndef MEMORIES_MODEL_VARIANT433_ENTRY_H
#define MEMORIES_MODEL_VARIANT433_ENTRY_H
#include "../../types.h"
#include "../french_model_variant/variant337_entry.h"

typedef struct {
    u8 unknown_00[4], part, unknown_05[3];
    u32 unknown_08;
    u32 trigger;
    u8 unknown_10[0x14];
} Variant433EntryConfig;

/* A centre and eight points on a circle, a colour and the sweep words. */
typedef struct {
    SVECTOR point[9];
    CVECTOR color;
    s32 field_4C, field_50, field_54, field_58, field_5C, field_60, field_64;
} Variant433EntryFan;

/* A 0x218-byte ribbon: nine phases, two colours, a scale and a travelling
 * target with its offset from the view origin. */
typedef struct {
    u8 unknown_000[0x120];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0x90];
    s32 phase[9];
    s32 completed;
    VECTOR target;
    VECTOR delta;
} Variant433EntryRibbon;

/* A 0x208-byte band or ring: two colours, a scale, a heading and nine amounts. */
typedef struct {
    u8 unknown_000[0x120];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0x48];
    s32 heading[3];
    u8 unknown_194[0x4C];
    s32 amount[9];
    s32 field_204;
} Variant433EntryBand;

/* A 0xA0-byte sheet of four quads like ModelVariantSheet with a larger tail. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    u8 outer[4], inner[4];
    s32 size, field_8C, field_90, field_94, field_98, field_9C;
} Variant433EntrySheet;

typedef struct {
    u8 unknown_000[0x264];
    CVECTOR color[17];
    s32 field_2A8;
    u8 unknown_2AC[0xCC];
} Variant433EntryStreamer;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant433EntryFan fans[6];
    Variant433EntryRibbon ribbons[6];
    Variant433EntryBand bands[3];
    Variant433EntryBand rings[16];
    Variant433EntrySheet sheets[7];
    Variant433EntryStreamer streamers[2];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2];
    POLY_FT4 flat_extra[2];
    u8 unknown_4230[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    s16 screen_x, screen_y;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step, fade;
    Variant433EntryConfig *G32 config;
    u8 unknown_42C0[4];
    GsCOORDUNIT *G32 part;
    s16 field_42C8, field_42CA, field_42CC, field_42CE;
    s32 field_42D0, field_42D4, field_42D8, field_42DC, field_42E0;
    s32 field_42E4, field_42E8, field_42EC, field_42F0, field_42F4;
    s32 phase;
    u8 unknown_42FC[8];
    s32 tint;
    s16 slot, command;
} Variant433EntryState;

extern u8 gModelVariant433_D39A0[];
void ModelVariant433_DrawRibbons(u8 *context);
void ModelVariant433_DrawRings(u8 *context);
void SpanishModelVariant450_DrawQuadGroups(u8 *context);
void SpanishModelVariant450_DrawLines(u8 *context);
#endif
