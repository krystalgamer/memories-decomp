#ifndef MEMORIES_DECOMP_MODEL_VARIANT823_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT823_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
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

/* One 0x24-byte command row: the beam colour, the model point it leaves
 * from, the head sprite size, the swing range, the swing period, the trail
 * edge offsets and depth, the impact sprite size, the flash size, the ring
 * radius, the spark length and spread, the fade steps and the start and end
 * times. */
typedef struct {
    u8 r, g, b;
    u8 part;
    u16 size;
    u16 swing;
    u16 period;
    s16 top;
    s16 bottom;
    s16 depth;
    u16 impact_size;
    u16 flash_size;
    u16 ring_radius;
    u16 spark_length;
    u16 spark_spread;
    u8 fade, unknown_1B;
    u8 trail_fade, unknown_1D;
    u16 unknown_1E;
    u16 start;
    u16 end;
} Model823Config;

/* The work area: the command row; for each of 36 beams the position,
 * origin, swing, 24-point trail and impact point; the ring under the
 * flash; each beam's sixteen sparks; the beam count, trail lengths, timers,
 * phase, impact flags, flash blink, frames, ambient level, textures, head
 * alpha, beam colours and the flash colour. */
typedef struct {
    Model823Config *G32 config;
    SVECTOR pos[36];
    SVECTOR origin[36];
    SVECTOR swing[36];
    SVECTOR trail[36][24];
    SVECTOR hit[36];
    SVECTOR ring[12];
    SVECTOR sparks[36][16];
    u16 count;
    u16 drawn[36];
    u32 elapsed;
    u32 frames;
    u16 phase;
    u16 hit_flag[36];
    u16 blink;
    u16 frame[36];
    s32 ambient;
    s32 texture[3];
    u8 alpha[36];
    u8 r[36];
    u8 g[36];
    u8 b[36];
    union {
        CVECTOR c;
        u32 word;
    } flash;
} Model823State;

extern GsIMAGE D_8013CEAC[];
extern Model823Config D_8013CF00[];

s32 func_8013B004(u8 *context, s32 command);

#endif
