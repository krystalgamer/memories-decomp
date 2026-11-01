#ifndef MEMORIES_DECOMP_MODEL_VARIANT106_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT106_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x3A-byte command row: the dome's base and rim colours, the sprite,
 * flash and fan colours, the two joints, the dome radii, ring radius and
 * height, the sprite and flash sizes, the fan arms, the timings and speeds. */
typedef struct {
    u8 base_red, base_green, base_blue, rim_red, rim_green, rim_blue;
    u8 sprite_red, sprite_green, sprite_blue, flash_red, flash_green, flash_blue;
    u8 unknown_0C[3];
    u8 fan_red, fan_green, fan_blue;
    s16 joint, fan_joint, inner_radius, outer_radius, ring_radius, sprite_size, flash_size;
    s16 fan_short, fan_long, height, move_duration, fan_duration, ring_speed, spin_speed, delay;
    s16 grow_end, fade_start, fade_end, dome_delay, flash_delay;
} Model106Config;

typedef struct {
    u8 r, g, b, pad;
} Model106Colour;

/* The 0x530-byte work area: the command row, the 8x16 dome, the eight ring
 * offsets, the 17-point fan, the 16 column colours, three textures, the
 * completion flag, the sprite frames and the elapsed time. */
typedef struct {
    Model106Config *G32 config;
    SVECTOR dome[128];
    SVECTOR ring[8];
    SVECTOR fan[17];
    Model106Colour colours[16];
    u32 texture[3];
    u8 completed;
    u8 frame[16];
    u8 unknown_529[3];
    s32 elapsed;
} Model106State;

extern GsIMAGE gModelVariant106_D2174[];
extern Model106Config gModelVariant106_D21C8[];

s32 ModelVariant106_HandleRequest(u8 *context, s32 command);

#endif
