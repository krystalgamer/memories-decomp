#ifndef FRENCH_MODEL_VARIANT436_ENTRY_H
#define FRENCH_MODEL_VARIANT436_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/variant436_sheets.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    s32 start, scale_start, scale_end;
    s32 phase2_end, phase3_end, travel_end;
    s32 wave_end, loop_end;
    u8 unknown_2C[4];
} EntryConfig436;

/* One 0x200-byte ribbon of nine points: the entry seeds style, and the
 * ribbon drawer projects the points and takes its colour from style.inner. */
typedef struct {
    SVECTOR a[9];
    ModelVariantScreenPoint sa[9];
    s32 angle[9];
    SVECTOR b[9];
    ModelVariantScreenPoint sb[9];
    s32 width[9];
    ModelVariantStripStyle style;
    u8 unknown_140[0x68];
    s32 otz[9];
    s16 ox[9];
    s16 oy[9];
    s32 start, end, state, count;
} EntryRibbon436;

typedef struct {
    EntryRibbon436 ribbons[4];
    Sheet436 sheets[1];
    ModelVariantBandTriple records[1];
    ModelVariantShortStreamer streamers[3];
    u8 unknown_1098[0x318];
    ModelVariantPackets packets;
    POLY_FT4 flat[2], extra[2];
    u8 unknown_14F8[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    MATRIX origin_a, origin_b;
    VECTOR direction;
    s32 factor, lift, scale, spread;
    VECTOR delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    s32 frame_count;
    s32 elapsed;
    u32 animation_frame;
    s32 step, span, fade;
    EntryConfig436 *config;
    u32 field_15F8;
    GsCOORDUNIT *parts[3];
    s16 field_1608, field_160A;
    s32 field_160C, width, field_1614, field_1618, spin;
    s16 field_1620, field_1622;
    s32 length, rotation_x, rotation_y, rotation_z;
    s32 phase_a, phase_b;
    ModelVariantEntryTail tail;
} EntryState436;

extern GsIMAGE gFrenchModelVariant436_aImages[9];
extern EntryConfig436 gFrenchModelVariant436_aConfigs[];
void FrenchModelVariant436_DrawRibbons(EntryState436 *context);
void SpanishModelVariant436_DrawSheets(EntryState436 *context);
void FrenchModelVariant436_DrawStreamers(EntryState436 *context);
s32 FrenchModelVariant436_HandleRequest(SVECTOR *point, s32 command);

#endif
