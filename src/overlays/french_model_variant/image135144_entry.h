#ifndef FRENCH_MODEL_IMAGE135144_ENTRY_H
#define FRENCH_MODEL_IMAGE135144_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

typedef struct {
    u8 r, g, b;
    u8 inner_r, inner_g, inner_b;
    u8 unknown_r, unknown_g, unknown_b;
    u8 part, aim_part, charge_part;
    u16 length, width, speed, count;
    u16 glow_size, puff_speed, puff_count, puff_size;
    u16 ray_width, ray_spread, ray_count;
    u8 fade, unknown_23, unknown_24, unknown_25;
    u16 charge_end, launch;
} Image135144Config;

typedef struct {
    Image135144Config *G32 config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR origin[8];
    SVECTOR puffs[8][32];
    SVECTOR puff_velocity[8][32];
    SVECTOR rays[8][32];
    SVECTOR trail[8][32];
    u32 elapsed;
    u16 phase;
    u16 arrived[8];
    u16 trail_head[8];
    u16 tail[8];
    s32 ambient;
    s32 texture[3];
    u8 level[8];
    u8 r[8], g[8], b[8];
    u8 inner_r[8], inner_g[8], inner_b[8];
    u8 unknown_r[8], unknown_g[8], unknown_b[8];
} Image135144State;

extern const VECTOR D_8013D18C;
extern GsIMAGE D_8013D19C[];
extern Image135144Config D_8013D1F0[];

s32 func_8013B004(u8 *context, s32 command);

#endif
