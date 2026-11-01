#ifndef MEMORIES_DECOMP_MODEL_VARIANT801_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT801_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

/* One 0x18-byte command row: the sprite half extents, the flight length,
 * the sprite count, the start circle radius, the spark spread, count and
 * size, the fade step, the start time and the time the sprites set off. */
typedef struct {
    u16 width;
    u16 height;
    u16 steps;
    u16 count;
    u16 radius;
    u16 spark_spread;
    u16 spark_count;
    u16 spark_size;
    u8 fade, unknown_11;
    u16 unknown_12;
    u16 start;
    u16 launch;
} Model801Config;

/* The work area: the command row; per sprite its position, step, angle,
 * target, 32-point trail and sparks; the timer, the spark frames, the
 * phase, the arrival flags, the drawn and moving counts, the trail lengths
 * and heads, the ambient level, the two textures and the sprite levels. */
typedef struct {
    Model801Config *G32 config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR target[8];
    SVECTOR trail[8][32];
    SVECTOR sparks[8][8];
    u32 elapsed;
    u16 spark_frame[8][8];
    u16 phase;
    u16 arrived[8];
    u16 shown;
    u16 moving;
    u16 tail[8];
    u16 trail_head[8];
    s32 ambient;
    s32 texture[2];
    u8 level[8];
} Model801State;

extern GsIMAGE gModelVariant801_D1554[];
extern Model801Config gModelVariant801_D158C[];

s32 ModelVariant801_HandleRequest(u8 *context, s32 command);

#endif
