#ifndef MEMORIES_DECOMP_MODEL_VARIANT777_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT777_ENTRY_H

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

/* One 0x30-byte command row: the bolt colour and the model point it starts
 * from, the bolt sprite size and jitter, the ray spread and width, the
 * debris and spark sizes and speeds, the points per bolt and the spawn
 * period, the tip scale, the two fade steps, and the timing of the start,
 * the last bolt, the end and both camera presets, and the flash switch. */
typedef struct {
    u8 r, g, b, part;
    u16 bolt_size;
    u16 jitter;
    u16 ray_spread;
    u16 ray_width;
    u16 debris_size;
    u16 debris_speed;
    u16 spark_size;
    u16 spark_speed;
    u16 point_count;
    u16 period;
    s32 tip_scale;
    u8 fade, unknown_1D, unknown_1E, unknown_1F;
    u8 tip_fade, unknown_21;
    u16 start;
    u16 spawn_end;
    u16 end;
    u16 first_camera, second_camera;
    u16 flash;
    u16 unknown_2E;
} Model777Config;

/* The work area: the command row, sixteen bolts of up to twelve points
 * (each with its ray-fan rotation and colour), the sixteen shared ray
 * directions, the eight debris sprites and sixteen sparks thrown from each
 * bolt tip with their velocities, the timers, the bolt ring head, the phase,
 * the per-bolt strike flags, lengths and debris frames, the camera mode, the
 * ambient level, the three textures and the flash level. */
typedef struct {
    Model777Config *G32 config;
    SVECTOR points[16][12];
    SVECTOR rays[16];
    SVECTOR spins[16][12];
    SVECTOR debris[16][8];
    SVECTOR debris_velocity[16][8];
    SVECTOR sparks[16][16];
    SVECTOR spark_velocity[16][16];
    u32 elapsed;
    u32 frames;
    u16 head;
    u16 phase;
    u16 struck[32];
    u16 unknown_24D0;
    u16 length[32];
    u16 debris_frame[16];
    u16 camera;
    s32 ambient;
    s32 texture[3];
    u8 red[16][12];
    u8 green[16][12];
    u8 blue[16][12];
    u8 flash;
} Model777State;

extern GsIMAGE D_8013D440[];
extern Model777Config D_8013D494[];

s32 func_8013B004(u8 *context, s32 command);

#endif
