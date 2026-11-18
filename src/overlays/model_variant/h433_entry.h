#ifndef MEMORIES_MODEL_VARIANT433_ENTRY_H
#define MEMORIES_MODEL_VARIANT433_ENTRY_H
#include "../../types.h"
#include "../model_variant/e503_h320_entry_pal.h"
#include "h433_lines.h"
#include "h433_quads.h"

typedef struct {
    u8 unknown_00[4], part, unknown_05[3];
    u32 unknown_08;
    u32 trigger;
    u8 unknown_10[0x10];
    /* After this frame the ring pulses stop restarting and hold. */
    u32 settle_start;
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
    SVECTOR points[9];
    ModelVariantScreenPoint screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    ModelVariantScreenPoint edge_screen[9];
    s32 width[9];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0x48];
    s32 depth[9];
    s16 x_offset[9];
    s16 y_offset[9];
    s32 phase[9];
    s32 completed;
    VECTOR target;
    VECTOR delta;
} Variant433EntryRibbon;


/* One 0x208-byte band or ring: nine points, their screen positions and edge
 * copies, the per-point angles and widths, the colours, scale and heading the
 * entry seeds, and per point the depth, screen offsets and growth amount. */
typedef struct {
    SVECTOR points[9];
    ModelVariantScreenPoint screen[9];
    s32 angle[9];
    SVECTOR edges[9];
    ModelVariantScreenPoint edge_screen[9];
    s32 width[9];
    CVECTOR color, secondary;
    VECTOR scale;
    s16 field_138, field_13A;
    s32 field_13C;
    u8 unknown_140[0x48];
    s32 heading[3];
    u8 unknown_194[4];
    s32 depth[9];
    s16 x_offset[9];
    s16 y_offset[9];
    s32 amount[9];
    s32 field_204;
} Variant433EntryBand;

/* A 0xA0-byte sheet of four quads like ModelVariantSheet with a larger tail. */
typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    CVECTOR outer;
    CVECTOR inner;
    s32 size, field_8C, field_90, field_94, fading, brightness;
} Variant433EntrySheet;

typedef struct {
    SVECTOR points[17];
    ModelVariantScreenPoint screen[17];
    s32 angle[17];
    SVECTOR edges[17];
    ModelVariantScreenPoint edge_screen[17];
    s32 width[17];
    u8 unknown_220[0x44];
    CVECTOR color[17];
    s32 field_2A8;
    s32 field_2AC[17];
    ModelVariantStripOffsets offsets;
} Variant433EntryStreamer;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant433EntryFan fans[6];
    Variant433EntryRibbon ribbons[6];
    Variant433EntryBand bands[3];
    Variant433EntryBand rings[16];
    Variant433EntrySheet sheets[7];
    Variant433EntryStreamer streamers[2];
    ModelVariantPackets packets;
    POLY_FT4 flat[2];
    POLY_FT4 flat_extra[2];
    u8 unknown_4230[0x24];
    MATRIX matrix;
    SVECTOR target;
    VECTOR direction;
    s16 screen_x, screen_y;
    VECTOR view_delta;
    ModelVariantClock clock;
    Variant433EntryConfig *config;
    u8 unknown_42C0[4];
    GsCOORDUNIT *part;
    s16 field_42C8, field_42CA, field_42CC, radius;
    s32 field_42D0;
    ModelVariantStreamerParams streamer;
    s32 ripple, ripple2, field_42EC, field_42F0, spin;
    ModelVariantEntryTail tail;
} Variant433EntryState;

extern GsIMAGE gModelVariant433_aImages[9];
extern Variant433EntryConfig gModelVariant433_aConfigs[];
void ModelVariant433_DrawRibbons(Variant433EntryState *context);
void ModelVariant433_DrawRings(Variant433EntryState *context);
void ModelVariant433_DrawBands(Variant433EntryState *context);
void ModelVariant433_DrawCoils(Variant433EntryState *context);
void SpanishModelVariant450_DrawQuadGroups(Variant450QuadView *context);
void SpanishModelVariant450_DrawLines(Variant450View *context);
#endif
