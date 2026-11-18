#ifndef FRENCH_MODEL_VARIANT437_ENTRY_H
#define FRENCH_MODEL_VARIANT437_ENTRY_H

#include "../../types.h"
#include "../model_variant/model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../model_variant/h420_sheet_pal.h"

typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    u32 start, scale_start, scale_end;
    u8 unknown_18[4];
    u32 move_start, move_end;
    u32 wave_end;
    u8 unknown_28[8];
} Variant437EntryConfig;

typedef struct {
    ModelVariantNinePointRibbon ribbons[4];
    ModelVariantTwoToneSheet sheets[1];
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
    u32 frame_count, elapsed, animation_frame;
    s32 step;
    u32 span, fade;
    Variant437EntryConfig *config;
    u32 field_15F8;
    GsCOORDUNIT *parts[3];
    s16 field_1608, field_160A;
    s32 field_160C, width, field_1614, field_1618, spin;
    s16 field_1620, field_1622;
    s32 length, rotation_x, rotation_y, rotation_z;
    s32 phase_a, phase_b;
    ModelVariantEntryTail tail;
} Variant437EntryState;

extern GsIMAGE gFrenchModelVariant437_aImages[9];
extern Variant437EntryConfig gFrenchModelVariant437_aConfigs[];
void SpanishModelVariant437_DrawSheet(Variant437EntryState *context);
void SpanishModelVariant437_DrawStreamers(Variant437EntryState *context);
s32 FrenchModelVariant437_HandleRequest(Variant437EntryState *context, s32 command);

#endif
