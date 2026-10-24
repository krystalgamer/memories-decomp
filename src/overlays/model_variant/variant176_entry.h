#ifndef MEMORIES_DECOMP_MODEL_VARIANT176_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT176_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_u16_values.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"
#include "../../game/screen_projection.h"

/* One 0x22-byte command row: the colours of the dots, the strips and the
 * sprites, the camera part (0xFF for a fixed point), the ring radii, and the
 * duration of each phase. */
typedef struct {
    u8 dot_red, dot_green, dot_blue, strip_red, strip_green, strip_blue;
    u8 sprite_red, sprite_green, sprite_blue, part;
    s16 dot_radius, outer_radius, inner_radius, depth, sprite_size, sprite_radius;
    s16 dot_duration, strip_duration, sprite_duration, dot_count, sprite_count, delay;
} Model176Config;

/* The work area: 128 dots and their velocities, the two 33-vertex rings the
 * strips are cut from, 32 sprite velocities, and the two texture words. */
typedef struct {
    Model176Config *G32 config;
    SVECTOR dots[128];
    SVECTOR dot_velocities[128];
    SVECTOR ring[66];
    SVECTOR sprite_velocities[32];
    s32 texture[2];
    u8 completed, unknown_B1D, unknown_B1E[2];
    s32 elapsed;
} Model176State;

extern GsIMAGE D_8013C3C4[];
extern Model176Config D_8013C3FC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
