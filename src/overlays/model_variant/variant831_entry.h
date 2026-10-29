#ifndef MEMORIES_DECOMP_MODEL_VARIANT831_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT831_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x2A-byte command row: the spark colour, the halo colour, the source
 * and target model parts, the glow, spark and halo sizes, the gather time,
 * the gather radius, the beam height and widths, the burst speed and spread,
 * the three fade steps, and the phase timings. */
typedef struct {
    u8 r, g, b;
    u8 halo_r, halo_g, halo_b;
    u8 part, target_part;
    u16 glow_size;
    u16 spark_size;
    u16 halo_size;
    u16 gather_time;
    u16 radius;
    u16 beam_height;
    u16 beam_top;
    u16 beam_base;
    u16 burst_speed;
    u16 spread;
    u8 spark_fade, pad_1d;
    u8 glow_fade, pad_1f;
    u8 core_fade, pad_21;
    u16 delay;
    u16 spawn_end;
    u16 burst_time;
    u16 gather_end;
} Model831Config;

/* The work area: the command row, the sparks, their velocities, an unused
 * block, the gathered position, the elapsed time, the frame, the phase, the
 * spark count, the glow scale, the ambient level, the core texture, the
 * textures, the glow level, the flash level, the spark colours, the core
 * colour and the halo colour. */
typedef struct {
    Model831Config *G32 config;
    SVECTOR sparks[64];
    SVECTOR velocities[64];
    SVECTOR unused[64];
    SVECTOR position;
    u32 elapsed;
    s32 frame;
    u16 phase;
    u16 count;
    u32 scale;
    s32 level;
    u16 core_tpage;
    u16 core_clut;
    s32 texture[4];
    u8 glow;
    u8 flash;
    u8 r[64];
    u8 g[64];
    u8 b[64];
    u8 core_r, core_g, core_b;
    u8 halo_r, halo_g, halo_b;
} Model831State;

extern GsIMAGE D_8013D2D0[];
extern Model831Config D_8013D340[];

s32 func_8013B004(u8 *context, s32 command);

#endif
