#ifndef MEMORIES_FRENCH_MODEL_VARIANT431_ENTRY_H
#define MEMORIES_FRENCH_MODEL_VARIANT431_ENTRY_H
#include "../../types.h"
#include "e503_h320_entry_pal.h"
#include "../model_variant/e575_fan.h"

typedef struct {
    u8 unknown_00[4], parts[5], unknown_09[11];
    u32 first_start;
    u8 unknown_18[0x14];
    u32 timings[15];
} Variant431EntryConfig;

typedef struct {
    SVECTOR a[5], b[5], c[5];
    long sa[5], sb[5], sc[5];
    CVECTOR ca[5], cb[5];
    VECTOR scale;
    s16 field_EC, field_EE;
    s32 field_F0, progress[5], width;
    s32 otz[5];
} Variant431EntryBand;

/* Minimum observed extent, not an allocation capacity. */
typedef struct {
    ModelVariantWebNarrow webs[3];
    Variant431EntryBand bands[5];
    ModelVariantSheet sheets[6];
    ModelVariantColorRing rings[6];
    ModelVariantColorRing spokes[4];
    Variant414Fan fans[5];
    ModelVariantPackets packets;
    POLY_GT4 extra;
    POLY_FT4 flat_textured[2];
    u8 unknown_1720[0x10];
    GsGLINE line;
    u8 unknown_1744[0x14];
    s32 position[3];
    SVECTOR target;
    VECTOR direction;
    DVECTOR screen_delta;
    MATRIX matrices[5];
    VECTOR directions[5];
    s16 screen_x[5], screen_y[5];
    VECTOR view_delta;
    s32 angles[2];
    u32 frame_count, frame, animation_frame;
    s32 step;
    u32 fade;
    Variant431EntryConfig *config;
    u32 field_18B4;
    GsCOORDUNIT *parts[5];
    s16 field_18CC, field_18CE, field_18D0, band_brightness;
    s32 field_18D4, start_index, spin;
    ModelVariantEntryTail tail;
} Variant431EntryState;

extern GsIMAGE gFrenchModelVariant431_aImages[9];
extern Variant431EntryConfig gFrenchModelVariant431_aConfigs[];
void FrenchModelVariant431_DrawWebs(Variant431EntryState *context);
void FrenchModelVariant431_DrawFans(Variant431EntryState *context);
void FrenchModelVariant431_DrawBands(Variant431EntryState *context);
void FrenchModelVariant431_DrawSheets(Variant431EntryState *context);
s32 FrenchModelVariant431_HandleRequest(Variant431EntryState *context, s32 command);
#endif
