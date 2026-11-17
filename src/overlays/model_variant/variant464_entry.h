#ifndef MEMORIES_MODEL_VARIANT464_ENTRY_H
#define MEMORIES_MODEL_VARIANT464_ENTRY_H
#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_camera_effects.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"

typedef struct {
    CVECTOR outer, inner;
    u8 unknown_08[8], part, unknown_11[7];
    u32 draw_start, field_1C, grow_start, grow_end;
    u8 unknown_28[0xC];
} Variant464EntryConfig;

/* A 0x8EC-byte band of 33 steps: a spine and an offset edge per step, their
 * packed screen words, per-step heading and length, the growth amount and
 * offset (staggered at -128 * step, growing to 0x800), two colours per step,
 * the scale, the direction the entry seeds, and per step the projection flag,
 * depth and the quads' half-width vector. */
typedef struct {
    SVECTOR spine[33];
    long screen[33];
    s32 heading[33];
    SVECTOR edge[33];
    long edge_screen[33];
    s32 length[33];
    s32 amount[33], offset[33];
    CVECTOR color[33], secondary[33];
    VECTOR scale;
    s16 field_640, field_642;
    s32 field_644;
    u8 unknown_648[0x108];
    s32 direction[3];
    u8 unknown_75C[4];
    long flag[33];
    s32 otz[33];
    s16 dx[33];
    s16 dy[33];
} Variant464EntryBand;

/* Minimum observed context extent, not an allocation capacity. */
typedef struct {
    ModelVariantCurtain funnels[2];
    Variant464EntryBand bands[1];
    ModelVariantDome dome;
    ModelVariantFlaggedSpiralArm spirals[12];
    ModelVariantSheet sheets[2];
    ModelVariantStreamer streamers[2];
    u8 unknown_1DD0[0x34];
    POLY_GT4 special;
    u8 unknown_1E38[0x34];
    POLY_GT4 beams[2];
    ModelVariantPackets packets;
    POLY_FT4 flat[2];
    u8 unknown_1FCC[0x10];
    MATRIX matrix;
    ModelVariantAim aim;
    ModelVariantClock clock;
    Variant464EntryConfig *config;
    u32 field_2048;
    GsCOORDUNIT *part;
    s16 field_2050, field_2052;
    s32 width, field_2058, field_205C, field_2060, field_2064;
    s32 field_2068, field_206C, field_2070, field_2074, field_2078;
    s32 field_207C, field_2080, progress;
    s16 field_2088, field_208A, field_208C, field_208E;
    s32 field_2090, field_2094, field_2098, field_209C;
    CVECTOR inner, outer;
    s32 spin, field_20AC, field_20B0, field_20B4, field_20B8, phase;
    s32 field_20C0, field_20C4;
    SVECTOR camera;
    s32 ambient;
    s16 slot, command;
} Variant464EntryState;

extern GsIMAGE gModelVariant464_aImages[8];
extern Variant464EntryConfig gModelVariant464_aConfigs[];
void FrenchModelVariant481_DrawBand(Variant464EntryState *context);
void FrenchModelVariant481_DrawRings(Variant464EntryState *context);
s32 ModelVariant464_HandleRequest(Variant464EntryState *context, s32 command);
#endif
