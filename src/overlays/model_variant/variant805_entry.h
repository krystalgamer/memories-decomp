#ifndef MEMORIES_DECOMP_MODEL_VARIANT805_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT805_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x38-byte command row: the bolt, glow and ring sprite colours, the
 * three model points (the bolt's source and the two aim points), the
 * charge sprite depth and half height, the flight length, the bolt count,
 * the impact glow size, the ring sprite count, size and spread, the ring
 * reveal rate, the ray width, spread and count, the flash width, top and
 * bottom, the two fade steps and the four timings: the charge end, the
 * launch, the flash start and the end. */
typedef struct {
    u8 r, g, b;
    u8 glow_r, glow_g, glow_b;
    u8 ring_r, ring_g, ring_b;
    u8 part;
    u8 aim_part;
    u8 charge_part;
    u16 depth;
    u16 size;
    u16 steps;
    u16 count;
    u16 glow_size;
    u16 ring_count;
    u16 ring_width;
    u16 ring_height;
    u16 ring_spread;
    u16 ring_rate;
    u16 ray_width;
    u16 ray_spread;
    u16 ray_count;
    u16 flash_width;
    u16 flash_top;
    u16 flash_bottom;
    u8 fade, unknown_2D;
    u8 ring_fade, unknown_2F;
    u16 charge_end;
    u16 launch;
    u16 flash_start;
    u16 end;
} Model805Config;

/* The work area: the command row; per bolt its position, step, angle,
 * target, ring offsets, rays and 16-point trail; the timer; the ring
 * animation frames; the phase, the arrival flags, trail heads and lengths
 * and revealed ring counts; the ambient level, the textures, the per-bolt
 * bolt and glow colours, the per-ring colours and the flash level. */
typedef struct {
    Model805Config *G32 config;
    SVECTOR pos[8];
    SVECTOR step[8];
    SVECTOR angle[8];
    SVECTOR target[8];
    SVECTOR ring[8][64];
    SVECTOR rays[8][32];
    SVECTOR trail[8][16];
    u32 elapsed;
    u16 frame[8][64];
    u16 phase;
    u16 arrived[8];
    u16 trail_head[8];
    u16 tail[8];
    u16 ring_shown[8];
    s32 ambient;
    s32 texture[3];
    u8 bolt_r[8];
    u8 bolt_g[8];
    u8 bolt_b[8];
    u8 glow_r[8];
    u8 glow_g[8];
    u8 glow_b[8];
    u8 ring_r[8][64];
    u8 ring_g[8][64];
    u8 ring_b[8][64];
    u8 flash;
} Model805State;

extern GsIMAGE D_8013DCAC[3];
extern Model805Config D_8013DD00[];

s32 func_8013B004(u8 *context, s32 command);

#endif
