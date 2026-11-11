#ifndef MEMORIES_DECOMP_MODEL_VARIANT632_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT632_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/camera_view.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

/* ModelVariantBurst here: one flare, sphere or trail strip: the 0x4C-byte
 * burst record of header 647. */

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
    ModelVariantBurst flares[32][2];
    ModelVariantBurst spheres[2];
    ModelVariantBurst strips[10];
} Model632State;

extern MATRIX GsIDMATRIX;
extern GsIMAGE gModelVariant632_aImages[2];
/* One 0xC-byte command row: the start delay, the radius, a value the entry
 * copies but never reads, the flare and trail colour, and the sphere
 * colour. */
extern ModelVariantSphereCommand gModelVariant632_aConfigs[];

s32 ModelVariant632_HandleRequest(u8 *context, s32 command);

#endif
