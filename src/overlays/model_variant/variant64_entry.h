#ifndef MEMORIES_DECOMP_MODEL_VARIANT64_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT64_ENTRY_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../game/model_state_setters.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x24-byte command row: the outer and inner ring colours, the model
 * part, the four ring radii, the ray radius, the ray sprite height and width,
 * the outer ring height, the fade length and the start, inner-ring end,
 * outer-ring end, fade start and finish times. */
typedef struct {
    u8 red, green, blue;
    u8 inner_red, inner_green, inner_blue;
    s16 part;
    s16 outer_bottom, outer_top, inner_bottom, inner_top, ray_radius;
    s16 ray_height, ray_width, outer_height, fade;
    s16 start, inner_end, outer_end, fade_start, finish;
} Model64Config;

/* The work area: the outer and inner ring vertex pairs, the rest and burst
 * ray offsets, the ring centre, the texture, the camera-shake stage and the
 * timer. */
typedef struct {
    Model64Config *G32 config;
    SVECTOR outer[66];
    SVECTOR inner[66];
    SVECTOR rays[8];
    SVECTOR bursts[8];
    SVECTOR center;
    s32 texture;
    u8 stage;
    s32 elapsed;
} Model64State;

extern GsIMAGE D_8013C1B8[1];
extern Model64Config D_8013C1D4[];

s32 func_8013B004(u8 *context, s32 command);

#endif
