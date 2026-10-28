#ifndef MEMORIES_DECOMP_MODEL_VARIANT97_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT97_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../game/screen_projection.h"

/* One 0x22-byte command row: the shard, glow and fan colours, the model part
 * the effect follows, the shard half size, the radii of the four rings, the
 * shard fade-in, fly-out, fan and flash times, the flash window, the gap
 * between shards, the start delay and the length of the glow. */
typedef struct {
    u8 red, green, blue;
    u8 glow_red, glow_green, glow_blue;
    u8 fan_red, fan_green, fan_blue;
    u8 part;
    u16 half_size;
    s16 shard_radius;
    s16 glow_radius;
    s16 fan_radius;
    s16 fade_in;
    s16 fly_time;
    s16 glow_ramp;
    s16 fan_time;
    s16 flash_time;
    s16 gap;
    s16 delay;
    s16 glow_time;
} Model97Config;

/* The 0x204-byte work area: the command row, the shard ring, the glow fan's
 * centre and ring, the shards' shared anchor, the outer fan's centre and its
 * two rings, four textures, the glow flicker, whether the hit has been
 * reported and the elapsed time. */
typedef struct {
    Model97Config *G32 config;
    SVECTOR shards[8];
    SVECTOR glow_centre;
    SVECTOR glow_ring[17];
    SVECTOR anchor;
    SVECTOR fan_ring[16];
    SVECTOR fan_centre;
    SVECTOR fan_outer[17];
    s32 texture[4];
    u16 frame;
    u8 hit;
    s32 elapsed;
} Model97State;

extern GsIMAGE D_8013C2F4[];
extern Model97Config D_8013C364[];

s32 func_8013B004(u8 *context, s32 command);

#endif
