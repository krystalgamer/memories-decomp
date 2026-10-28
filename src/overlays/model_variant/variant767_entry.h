#ifndef MEMORIES_DECOMP_MODEL_VARIANT767_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT767_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x4C-byte command row: the ring colour, the smoke speed, size, count and
 * rate, the ring radii and depth, the star size, the debris and ember sizes,
 * counts and speeds, the start delay, the times of the two bursts, the shake
 * and the flash flag. */
typedef struct {
    u8 r, g, b, pad_3;
    u16 smoke_speed;
    s16 smoke_size;
    u16 smoke_max;
    u16 smoke_rate;
    u16 inner_radius, outer_radius, ring_depth;
    u16 star_size;
    s16 debris_size;
    u16 debris_count;
    u16 debris_speed;
    u16 ember_count;
    s16 ember_size;
    u16 ember_speed;
    u16 unknown_20[9];
    u16 delay;
    u16 unknown_34;
    u16 first_burst_time;
    u16 second_burst_time;
    u16 unknown_3A[5];
    u16 shake_value, shake_arg;
    u16 flash;
    u16 unknown_4A;
} Model767Config;

/* A colour that the entry tests as a 24-bit word to know whether it faded out. */
typedef union {
    CVECTOR rgb;
    u32 word;
} Model767Color;

/* The work area: the command row and, per burst, the smoke puffs, debris and
 * embers and their velocities, the smoke frames and the ring scale; the shared
 * ring edges, timers, ambient level, textures and flash colour. */
typedef struct {
    Model767Config *G32 config;
    SVECTOR smoke[64][2];
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR debris[8][2];
    SVECTOR debris_velocities[8][2];
    SVECTOR embers[64][2];
    SVECTOR ember_velocities[64][2];
    u16 frames[64][2];
    u16 debris_frame[2];
    u16 elapsed;
    u16 tick;
    u16 phase;
    u16 smoke_count[2], unknown_F12;
    u32 ring_scale[2];
    s32 level;
    s32 texture[3];
    u16 burst_tpage, burst_clut;
    Model767Color color[2], flash;
} Model767State;

extern GsIMAGE D_8013C7B4[];
extern Model767Config D_8013C808[];

s32 func_8013B004(u8 *context, s32 command);

#endif
