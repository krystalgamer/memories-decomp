#ifndef MODEL_VARIANT769_ENTRY_H
#define MODEL_VARIANT769_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* 60-byte command record; the command index selects one directly. */
typedef struct {
    u8 wing_red, wing_green, wing_blue;
    u8 ring_red, ring_green, ring_blue;
    u8 slot;
    u8 ring_slot;
    s16 half_size;
    s16 height;
    s16 particle_count;
    s16 particle_radius;
    s16 particle_speed;
    s16 rise_speed;
    s16 travel_steps;
    s16 wing_count;
    s32 wing_height;
    s16 wing_width;
    s16 burst_count;
    s16 burst_speed;
    s16 burst_radius;
    s16 ring_radius;
    s16 outer_radius;
    s16 spray_radius;
    s16 spray_count;
    s16 start;
    s16 release;
    s16 launch_end;
    s16 unknown_32;
    s32 burst_time;
    s32 unknown_38;
} Model769Command;

typedef struct {
    Model769Command *G32 command;
    SVECTOR position;
    SVECTOR velocity;
    SVECTOR particles[128];
    SVECTOR particle_velocities[128];
    SVECTOR drift[128];
    SVECTOR ring[16];
    SVECTOR outer[16];
    SVECTOR spray[32];
    SVECTOR spray_velocities[32];
    DVECTOR spray_screen[32];
    s32 texture[2];
    s32 ambient;
    s32 elapsed;
    s32 growth;
    s16 state;
    s16 shown;
    u16 spray_depth[32];
    u8 fade;
    u8 flash;
    u8 burst_level;
    u8 ring_red, ring_green, ring_blue;
} Model769State;

/* Two GsIMAGE descriptors: the orb texture and the ring texture. */
extern GsIMAGE gModelVariant769_D2314[];
extern Model769Command gModelVariant769_D234C[];

#endif
