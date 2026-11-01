#ifndef MEMORIES_DECOMP_MODEL_VARIANT807_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT807_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 0x34-byte command row: the glow and halo colours, the model part the
 * effect starts from, the glow, halo and orbiting sprite sizes, the orbiter
 * count, spread and speed, the ring radius and height, the spark and
 * fragment sizes, the fragment spread and count, the core size, the four
 * fade steps, and the timing of the start, the ring collapse and the
 * camera preset. */
typedef struct {
    u8 r, g, b;
    u8 halo_r, halo_g, halo_b;
    u8 part, unknown_7;
    u16 glow_size;
    u16 spread;
    u16 halo_size;
    u16 count;
    u16 orbiter_size;
    u16 speed;
    u16 radius;
    u16 unknown_16;
    u32 height;
    u16 spark_size;
    u16 fragment_size;
    u16 fragment_spread;
    u16 fragment_count;
    u16 core_size;
    u8 spark_fade, unknown_27;
    u8 core_fade, unknown_29;
    u8 halo_fade, unknown_2B;
    u8 glow_fade, unknown_2D;
    u16 start;
    u16 collapse;
    u16 camera_end;
} Model807Config;

/* The work area: the orbiting points with their offsets and velocities, the
 * ring of paired points, the fan of rim points, the spark offsets, the
 * fragments of each spark and their velocities, the centre on the opposing
 * slot, the timer, the phase, the spark count, the ring scales, the ambient
 * level, the textures, the glow and halo colours, the flash level and the
 * fading spark colours. */
typedef struct {
    Model807Config *G32 config;
    SVECTOR points[16];
    SVECTOR offsets[16];
    SVECTOR velocities[16];
    SVECTOR ring[16][2];
    SVECTOR rim[16];
    SVECTOR sparks[32];
    SVECTOR fragments[32][16];
    SVECTOR fragment_velocities[32][16];
    SVECTOR center;
    u32 elapsed;
    u16 phase;
    u16 spark_count;
    u32 height_scale;
    u32 width_scale;
    s32 level;
    s32 texture[2];
    u8 glow_r, glow_g, glow_b;
    u8 flash;
    u8 halo_r, halo_g, halo_b;
    u8 spark_r[32];
    u8 spark_g[32];
    u8 spark_b[32];
} Model807State;

extern GsIMAGE gModelVariant807_D2BA8[2];
extern Model807Config gModelVariant807_D2BE0[];

s32 ModelVariant807_HandleRequest(u8 *context, s32 command);

#endif
