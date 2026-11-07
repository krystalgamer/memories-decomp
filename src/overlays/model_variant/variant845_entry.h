#ifndef MEMORIES_DECOMP_MODEL_VARIANT845_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT845_ENTRY_H

#include "../../types.h"
#include "model_variant.h"
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
#include "../../psyq/abs.h"

/* The work area: the command row; for each of the two bolts its position, step, strip
 * angle, ring offsets, target, 24-point trail, origin, smoke puffs and
 * their velocities and impact rays; the sixteen inner and outer fan
 * points; the sparks; the fan growth, the timer, the flight counters, the
 * frame count, puff and spark frames, trail lengths, spark counts and
 * trail heads; the launch flag, the arrival flags, the phase, the ambient
 * level, the textures and the per-bolt bolt, glow and ray colours. */
typedef struct {
    ModelVariantBoltFanConfig *config;
    SVECTOR pos[2];
    SVECTOR step[2];
    SVECTOR angle[2];
    SVECTOR ring[2][18];
    SVECTOR target[2];
    SVECTOR trail[2][24];
    SVECTOR origin[2];
    SVECTOR puffs[2][8];
    SVECTOR puff_velocity[2][8];
    SVECTOR rays[2][32];
    SVECTOR fan[16][2];
    SVECTOR sparks[2][16];
    u32 growth;
    u32 elapsed;
    u32 flight[2];
    u32 frames;
    u16 puff_frame[2][8];
    u16 spark_frame[2][8];
    u16 tail[2];
    u16 unknown_84C;
    u16 spark_count[2];
    u16 trail_head[2];
    u16 head;
    u16 arrived[2];
    u16 phase;
    s32 ambient;
    s32 texture[5];
    u8 bolt_r[2];
    u8 bolt_g[2];
    u8 bolt_b[2];
    u8 glow_r[2];
    u8 glow_g[2];
    u8 glow_b[2];
    u8 ray_r[2];
    u8 ray_g[2];
    u8 ray_b[2];
} Model845State;

extern GsIMAGE gModelVariant845_aImages[];
extern ModelVariantBoltFanConfig gModelVariant845_aConfigs[];

s32 ModelVariant845_HandleRequest(u8 *context, s32 command);

#endif
