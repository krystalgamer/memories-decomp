#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT388_ENTRY_H
#include "../../types.h"
#include "../../overlays/model_variant/e503_h320_entry_pal.h"
#include "../../game/model_slot_getters.h"

/* One 0x24-byte command record of the table at gSpanishModelVariant388_aConfigs. */
/* One 0x74-byte ribbon of this family: the two-point spine, its sideways
 * copy, both projections, per point the screen angle, projected width and
 * offset, and the RotTransPers4 flag kept for the draw pass. */
typedef struct {
    SVECTOR points[2];
    long screen[2];
    s32 angle[2];
    SVECTOR edges[2];
    long edge_screen[2];
    s32 width[2];
    u8 unknown_40[0x1C];
    long flag[2];
    s32 otz[2];
    u16 x_offset[2];
    u16 y_offset[2];
} Ribbon388;

typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u32 sheet_start, grow_end, strip_end;
    u32 track_end, shrink_end;
} Variant388EntryConfig;

/* One 0xF8-byte tube: five cross sections of three points each, their
 * packed screen positions, the colour and scale the entry seeds, and the
 * projection flags and depths. */
typedef struct {
    SVECTOR a[5];
    SVECTOR b[5];
    SVECTOR c[5];
    long sa[5];
    long sb[5];
    long sc[5];
    CVECTOR color;
    s32 scale[3];
    u8 unknown_C4[4];
    s16 field_C8, field_CA;
    s32 field_CC;
    long flag[5];
    long otz[5];
} Variant388EntryTube;

typedef struct {
    SVECTOR points[3][17];
    CVECTOR inner, outer;
    s32 progress, field_1A4;
} Variant388EntryFeedback;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    Variant388EntryTube tubes[1];
    ModelVariantSheet sheets[2];
    Ribbon388 ribbons[8];
    Variant388EntryFeedback feedback[5];
    ModelVariantColorRing spokes[4];
    ModelVariantFlaggedSpiralArm arms[12];
    ModelVariantPackets packets;
    POLY_GT4 paged;
    POLY_GT4 plain;
    POLY_FT4 flat_textured[2];
    u8 unknown_17E0[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant388EntryConfig *config;
    u8 unknown_1870[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    s16 spiral_angle, field_1886, spiral_size, spiral_shrink;
    s16 field_188C, field_188E, progress, strip_width;
    s32 ribbon_angle;
    s32 phase;
    s32 field_189C;
    u8 unknown_18A0[8];
    s32 tint;
    s16 slot, command;
} Variant388EntryState;

extern GsIMAGE gSpanishModelVariant388_aImages[9];
extern Variant388EntryConfig gSpanishModelVariant388_aConfigs[];
void FrenchModelVariant388_DrawStrips(Variant388EntryState *context);
void SpanishModelVariant388_DrawRibbonFan(Variant388EntryState *context);
void SpanishModelVariant388_DrawSheets(Variant388EntryState *context);
void SpanishModelVariant388_DrawFeedbackStrips(Variant388EntryState *context);
void FrenchModelVariant388_DrawPetals(Variant388EntryState *context);
typedef char Variant388EntryConfigSize[(sizeof(Variant388EntryConfig) == 0x24) ? 1 : -1];
typedef char Variant388EntryStateSize[(sizeof(Variant388EntryState) == 0x18B0) ? 1 : -1];
#endif
