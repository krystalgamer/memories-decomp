#ifndef MEMORIES_FRENCH_MODEL_VARIANT402_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT402_ENTRY_H
#include "../../types.h"
#include "variant402_bands.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

typedef struct {
    u8 unknown_00[4];
    u8 parts[3];
    u8 unknown_07[5];
    u16 part_count;
    u8 unknown_0E[2];
    u32 start;
} Variant402EntryConfig;

/* The strip record: three pairs of points, their screen positions, two
 * words the entry sets, and the two strip depths the strip drawer reads. */
typedef struct {
    SVECTOR points[3][2];
    long projected[3][2];
    s16 field_48, field_4A;
    s32 field_4C;
    s32 depth[2];
} Variant402EntryRecord;

typedef struct {
    SVECTOR points[16];
    s32 field_80, field_84, field_88, field_8C;
} Variant402EntryRing;

/* One of the eight ribbons the 402 ribbon drawer projects: two spine points
 * and their screen positions, angles and widths, two edge points and their
 * screen positions, depths and screen offsets. */
typedef struct {
    SVECTOR a[2];
    long sa[2];
    s32 angle[2];
    SVECTOR b[2];
    long sb[2];
    s32 width[2];
    s32 otz[2];
    u16 ox[2];
    u16 oy[2];
    u8 unknown_50[8];
} Family402Ribbon;

typedef struct {
    Variant402EntryRecord records[1];
    Variant402EntryRing rings[2];
    Family402Ribbon ribbons[8];
    ModelVariantCurtain bands[2];
    ModelVariantPackets packets;
    POLY_GT4 extra, band_packet;
    POLY_FT4 flat_textured[2];
    u8 unknown_7C8[0x814 - 0x7C8];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant402EntryConfig *config;
    u32 field_880;
    GsCOORDUNIT *parts[3];
    s32 part_index;
    CVECTOR inner, outer;
    s16 field_89C, field_89E, field_8A0, field_8A2;
    s16 width, displacement, ribbon_angle, field_8AA;
    s32 phase, field_8B0, field_8B4, field_8B8, tint;
    s16 slot, command;
} Variant402EntryState;

extern GsIMAGE gFrenchModelVariant402_aImages[9];
extern Variant402EntryConfig gFrenchModelVariant402_aConfigs[];
#endif
