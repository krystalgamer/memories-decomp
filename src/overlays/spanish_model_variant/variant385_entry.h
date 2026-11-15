#ifndef MEMORIES_DECOMP_SPANISH_MODEL_VARIANT385_ENTRY_H
#define MEMORIES_DECOMP_SPANISH_MODEL_VARIANT385_ENTRY_H
#include "../../types.h"
#include "../../overlays/french_model_variant/variant337_entry.h"
#include "../../game/model_slot_getters.h"

/* One 0x4C-byte command record of the table at gSpanishModelVariant385_aConfigs. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    s32 width;
    s32 inner_radius, outer_radius;
    s32 field_1C;
    s32 field_20;
    u16 ribbon_radius;
    u8 unknown_026[0x2];
    s32 size;
    u32 start;
    s32 field_30;
    u32 expansion_end, fade_start, fade_end;
    s32 offset[3];
} Variant385EntryConfig;

typedef struct {
    u8 unknown_00[0x6C];
    s16 field_6C, field_6E;
    s32 field_70;
    u8 unknown_74[0x18];
} Variant385EntryTube;

typedef struct {
    SVECTOR v0[4], v1[4], v2[4], v3[4];
    s32 size, field_84, field_88;
    u8 unknown_8C[4];
} Variant385EntrySheet;

/* Minimum observed extent, not an allocation capacity. */
/* One 0x60-byte ribbon of this family: the MODEL402 ribbon with the
 * projection flags kept between the widths and the depths. */
typedef struct {
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    long flag[2];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
    u8 unknown_58[8];
} Ribbon385;

typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant385EntryTube tubes[1];
    Variant385EntrySheet sheets[2];
    Ribbon385 ribbons[8];
    ModelVariantCurtain curtains[4];
    ModelVariantPackets packets;
    POLY_GT4 plain;
    POLY_GT4 paged;
    POLY_FT4 flat_textured[2];
    u8 unknown_F4C[0x4C];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant385EntryConfig *config;
    u8 unknown_1004[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    u8 color[3], unknown_101B;
    u8 outer_r, outer_g, outer_b, unknown_101F;
    s16 field_1020, field_1022, field_1024, field_1026;
    s16 radius, progress, ribbon_angle, ring_angle;
    s32 phase;
    s32 field_1034;
    u8 unknown_1038[8];
    s32 tint;
    s16 slot, command;
} Variant385EntryState;

extern GsIMAGE gSpanishModelVariant385_aImages[9];
extern Variant385EntryConfig gSpanishModelVariant385_aConfigs[];
void SpanishModelVariant385_DrawLines(u8 *context);
void SpanishModelVariant385_DrawStationStrip(u8 *context);
void FrenchModelVariant385_DrawRibbonFan(u8 *context);
void SpanishModelVariant385_DrawPulsingQuads(u8 *context);
void SpanishModelVariant385_DrawRings(u8 *context);
typedef char Variant385EntryConfigSize[(sizeof(Variant385EntryConfig) == 0x4C) ? 1 : -1];
typedef char Variant385EntryStateSize[(sizeof(Variant385EntryState) == 0x1048) ? 1 : -1];
#endif
