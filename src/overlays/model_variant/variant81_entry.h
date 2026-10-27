#ifndef MEMORIES_DECOMP_MODEL_VARIANT81_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT81_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/screen_projection.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x1E-byte command row: the model part the ribbon leaves from, the
 * ribbon's swirl radius, wave and twist, the cylinder radius and height, the
 * ribbon length and pattern count, the cylinder spin and grow times, the
 * start delay and the time after which ribbon segments stop being drawn. */
typedef struct {
    u8 unknown_0[3];
    u8 part;
    s16 swirl_radius;
    s16 wave_amplitude;
    s16 ring_radius;
    s16 ring_height;
    s16 segments;
    s16 count;
    s16 wave_period;
    s16 twist;
    s16 unknown_14;
    s16 spin_duration;
    s16 grow_duration;
    s16 start_delay;
    s16 trail_end;
} Model81Config;

/* The work area: the command row, the bottom and top rings of the cylinder,
 * the per-pattern vertical offsets, swirl angles and texture frames, the
 * ribbon gradient, the packed texture page and palette, the elapsed time and
 * whether the end has been reported. */
typedef struct {
    Model81Config *G32 config;
    SVECTOR bottom[33];
    SVECTOR top[33];
    s32 offsets[128];
    s16 angles[128];
    u8 frames[128];
    CVECTOR colours[129];
    s32 texture;
    s32 elapsed;
    u8 done;
} Model81State;

extern GsIMAGE D_8013C9C0;
extern Model81Config D_8013C9DC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
