#ifndef MEMORIES_MODEL_VARIANT432_ENTRY_H
#define MEMORIES_MODEL_VARIANT432_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../game/model_texture_upload.h"
#include "../../game/camera_view.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../spanish_model_variant/variant437_sheet.h"
#include "variant437_streamers.h"

/* One 0x24-byte command row at gModelVariant432_aConfigs. */
typedef struct {
    u8 unknown_00[4], parts[3], unknown_07[5];
    s32 start, scale_start, scale_end;
    s32 move_start, move_end, fade_start;
} Variant432EntryConfig;

/* Four 0x380-byte ribbons at the start of the work area. */
typedef struct {
    u8 unknown_000[0x220];
    CVECTOR inner, outer;
    VECTOR scale;
    s16 field_238, field_23A;
    s32 field_23C;
    u8 unknown_240[0x130];
    s32 start, end, state, count;
} Variant432EntryRibbon;

typedef struct {
    Variant432EntryRibbon ribbons[4];
    Sheet437 sheets[1];
    ModelVariantBandTriple records[1];
    Variant437Streamer streamers[3];
    u8 unknown_1698[0x318];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_GT4 textured[2];
    POLY_FT4 flat[2], extra[2];
    u8 unknown_1AF8[0x10];
    MATRIX matrix;
    VECTOR origin;
    SVECTOR target;
    MATRIX origin_a, origin_b;
    VECTOR direction;
    s32 factor, field_1B94, scale, spread;
    VECTOR delta, path;
    DVECTOR screen_delta;
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count;
    s32 elapsed;
    u32 animation_frame;
    s32 step;
    s32 span, fade;
    Variant432EntryConfig *config;
    u32 field_1BF8;
    GsCOORDUNIT *parts[3];
    s16 field_1C08, field_1C0A;
    s32 field_1C0C, field_1C10, field_1C14, field_1C18, spin;
    s16 field_1C20, field_1C22;
    s32 length, rotation_x, rotation_y, rotation_z;
    s32 phase_a, phase_b;
    ModelVariantEntryTail tail;
} Variant432EntryState;

extern GsIMAGE gModelVariant432_aImages[9];
extern Variant432EntryConfig gModelVariant432_aConfigs[];
void FrenchModelVariant449_DrawSheet(u8 *context);
void FrenchModelVariant449_DrawRibbons(u8 *context);
void FrenchModelVariant449_DrawStreamers(u8 *context);
s32 ModelVariant432_HandleRequest(SVECTOR *point, s32 command);

#endif
