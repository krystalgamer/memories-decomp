#ifndef MEMORIES_DECOMP_MODEL_VARIANT632_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT632_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* One flare, sphere or trail strip: the 0x4C-byte burst record of header
 * 647. */
typedef struct {
    u8 active, red, green, blue;
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration;
    GsCOORDUNIT *coord;
    SVECTOR rotation;
    VECTOR scale;
    VECTOR position;
    VECTOR unknown_3C;
} Model632Burst;

/* Two points tracked on the model in one frame; the trail strips span them. */
typedef struct {
    SVECTOR first, second;
} Model632TrailPoint;

/* The work area: the command, the radius, the two textures, the shared flare,
 * dot and trail packets, the flare corners, the 9x9 sphere points, eleven
 * frames of tracked points, the flags, the flare, copy and sphere counts, and
 * the flares, the spheres and the ten trail strips. */
typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot;
    s16 delay;
    u16 radius, unknown_0C;
    u16 tpage[2], unknown_12;
    u16 clut[2], unknown_18[2];
    POLY_FT4 flat;
    GsLINE dot;
    POLY_GT4 strip;
    SVECTOR square[4];
    SVECTOR grid[9][9];
    Model632TrailPoint trail[11];
    u8 flags[5];
    u8 flare_count, copies, sphere_count;
    Model632Burst flares[32][2];
    Model632Burst spheres[2];
    Model632Burst strips[10];
} Model632State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant632_aImages[2];
/* One 0xC-byte command row: the start delay, the radius, a value the entry
 * copies but never reads, the flare and trail colour, and the sphere
 * colour. */
extern ModelVariantSphereCommand gModelVariant632_aConfigs[];

s32 ModelVariant632_HandleRequest(u8 *context, s32 command);

#endif
