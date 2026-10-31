#ifndef MEMORIES_DECOMP_IMAGE60348_ENTRY_H
#define MEMORIES_DECOMP_IMAGE60348_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_update_view_metrics.h"
#include "../../game/model_queue_tint_request.h"
#include "../../game/model_state_setters.h"
#include "../../game/camera_view.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"

typedef struct {
    s16 delay, release;
    u16 radius, height;
    u8 color[6];
} Image60348Config;

typedef struct {
    u8 active, color[3];
    u8 u, v, unknown_06[2];
    s32 mode;
    u16 elapsed, duration, echo, released;
    GsCOORDUNIT *G32 coord;
    SVECTOR rotation;
    VECTOR scale, position, target;
} Image60348Element;

typedef struct {
    u8 command, unknown_01;
    u16 elapsed;
    s32 slot, direction;
    s16 delay;
    u16 radius, height, tpage[3], clut[3], unknown_1E;
    POLY_FT4 sprite;
    POLY_GT4 strip;
    POLY_GT3 shard;
    SVECTOR inner[33], outer[33], corners[4], ring[17], rim[17];
    SVECTOR spikes[4], flash_corners[4];
    SVECTOR cone_inner[17], cone_outer[17], cone_middle[17];
    u8 flags[5], ring_count, echo_count;
    ModelTintColor tint_start, tint_end;
    /* Echo rows retain one age slot for each of the eight rings. */
    Image60348Element rings[8], echoes[16][8], bursts[8];
} Image60348State;

typedef struct {
    s16 unknown_00, yaw, pitch, unknown_06;
} Image60348ViewSnapshot;

extern MATRIX D_800FE168;
extern GsIMAGE D_8013D6C8[3];
extern Image60348Config D_8013D71C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
