#ifndef MEMORIES_MODEL_VARIANT366_ENTRY_H
#define MEMORIES_MODEL_VARIANT366_ENTRY_H
#include "../../types.h"
#include "model_variant.h"
#include "../../game/camera_view.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"

/* One 0x18-byte command record of the table at gModelVariant366_aConfigs. */
typedef struct {
    u8 unknown_00[4];
    u8 parts[3], unknown_07[5];
    u16 count, unknown_0E;
    u32 duration;
    u32 end;
} Variant366Config;



typedef struct {
    ModelVariantScaledStrip strips[1];
    ModelVariantQuadSheet quads[1];
    ModelVariantRibbonSpan ribbon_spans[8];
    ModelVariantConeRings ribbons[3];
    ModelVariantColorRing rings[4];
    ModelVariantShadedRay rays[16];
    POLY_G3 triangle;
    POLY_G4 quad;
    POLY_F4 mask;
    POLY_GT4 textured[2], plain, paged;
    POLY_FT4 flat[2];
    u8 unknown_14AC[0x24];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant366Config *config;
    u8 unknown_153C[4];
    GsCOORDUNIT *parts[3];
    s32 index;
    s32 scale, brightness;
    s32 cone_opening[3];
    s16 ray_polar_angle, ray_azimuth, field_1568, field_156A, ray_radius, field_156E;
    s16 field_1570, field_1572, field_1574, unknown_1576;
    s16 travel, width;
    s32 spin, phase, field_1584;
    u8 unknown_1588[8];
    s32 tint;
    s16 slot, command;
} Variant366State;

extern GsIMAGE gModelVariant366_aImages[];
extern Variant366Config gModelVariant366_aConfigs[];

s32 ModelVariant366_HandleRequest(Variant366State *context, s32 command);
void FrenchModelVariant383_DrawStrip(Variant366State *context);
void FrenchModelVariant383_DrawRibbons(Variant366State *context);
void FrenchModelVariant383_DrawQuads(Variant366State *context);
void ModelVariant366_DrawCones(Variant366State *context);
void FrenchModelVariant383_DrawRays(Variant366State *context);
#endif
