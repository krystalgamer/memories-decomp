#ifndef MODEL_VARIANT773_ENTRY_H
#define MODEL_VARIANT773_ENTRY_H

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

/* One point of the position, velocity and grid arrays: the vector, or its
 * components by index, which is how the original addresses vy/vz. */
typedef union {
    SVECTOR v;
    s16 c[4];
} Model773Point;

typedef struct {
    u8 trail_red, trail_green, trail_blue;
    u8 ribbon_red, ribbon_green, ribbon_blue;
    u8 slot;
    u8 pad_07;
    s16 ribbon_width;
    s16 sprite_height;
    s16 travel_steps;
    s16 spread_x;
    s16 spread_y;
    s16 batch_count;
    s16 trail_radius;
    s16 trail_period;
    s16 flash_size;
    s16 burst_size;
    s16 start;
    s16 phase2;
    s16 phase3;
    s16 phase4;
    s16 flash_start;
    s16 phase_end;
} Model773Command;

typedef struct {
    Model773Command *G32 command;
    Model773Point pos[12];
    Model773Point vel[12];
    union {
        Model773Point grid[12][12];
        Model773Point flat[144];
    } offsets;
    union {
        Model773Point grid[12][12];
        Model773Point flat[144];
    } directions;
    SVECTOR ribbon[12][32];
    s16 flash[12];
    s16 burst[12];
    s16 state;
    s16 landed[12];
    s16 done;
    s16 timer;
    s16 count;
    s16 head[12];
    s32 texture[3];
    s32 ambient;
    u8 red[12];
    u8 green[12];
    u8 blue[12];
} Model773State;

extern GsIMAGE D_8013D518[];
extern Model773Command D_8013D56C[];

#endif
