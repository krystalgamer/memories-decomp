#ifndef MEMORIES_DECOMP_MODEL_VARIANT85_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT85_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_state_updates.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1E-byte command row: the triangle and sprite colours, the model part
 * the effect follows, the triangle height and width, the funnel's inner and
 * outer radii, the sprite half size, the burst duration, the triangle count,
 * the start delay, the end of the spin and the start of the burst. */
typedef struct {
    u8 red, green, blue;
    u8 sprite_red, sprite_green, sprite_blue;
    u8 part, unknown_07;
    s16 height;
    s16 width;
    s16 unknown_0C;
    s16 inner_radius;
    s16 outer_radius;
    s16 size;
    s16 duration;
    s16 count;
    s16 delay;
    s16 spin_end;
    s16 burst_start;
} Model85Config;

/* The 0x428-byte work area: the command row, each triangle's random start
 * angle and spin speed, the funnel's outer and inner rings, three textures,
 * a frame counter, whether the hit has been reported and the elapsed time. */
typedef struct {
    Model85Config *G32 config;
    SVECTOR angles[32];
    SVECTOR speeds[32];
    SVECTOR rings[66];
    s32 texture[3];
    u8 frame;
    u8 hit;
    s32 elapsed;
} Model85State;

/* The projection scratch that follows the three sprite vertices: it holds the
 * fourth sprite corner, and later the funnel's 66 projected ring points. */
typedef union {
    SVECTOR corner;
    DVECTOR screen[66];
} Model85Scratch;

extern GsIMAGE D_8013BF54[];
extern Model85Config D_8013BFA8[];

s32 func_8013B004(u8 *context, s32 command);

#endif
