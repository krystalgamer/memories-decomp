#ifndef MEMORIES_DECOMP_MODEL_VARIANT762_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT762_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x30-byte command row: the colour of the sparks and of the fan, the
 * attachment part, the radius of the sparks and of the fan rim, the spark
 * lifetime, the flash growth, the spark stagger, the half size of the rising
 * wings and their climb, the wing growth, the shard spread, the half length
 * of the shard base, the shard count, the dot speed and count, the inner and
 * outer ring radii, the start delay, the end of the sparks, the release time
 * and an unused end. */
typedef struct {
    u8 r, g, b, unknown_03;
    u8 unknown_04, unknown_05;
    u8 part, unknown_07;
    s16 spark_radius;
    s16 spark_life;
    s16 flash_growth;
    s16 spark_stagger;
    s16 wing_half;
    s16 wing_climb;
    s32 wing_growth;
    s16 unknown_18;
    s16 shard_spread;
    s16 base_half;
    s16 shard_count;
    s16 dot_speed;
    s16 dot_count;
    s16 inner_radius;
    s16 outer_radius;
    s16 delay;
    s16 spark_end;
    s16 release;
    s16 unknown_2E;
} Model762Config;

/* The three fading colours; the end test reads them as two words and a
 * byte. */
typedef union {
    struct {
        u8 color[3];
        u8 color2[3];
        u8 color3[3];
    } c;
    struct {
        s32 w0;
        s32 w1;
        u8 last;
    } w;
} Model762Colors;

/* The base of the shards shares its array with the shard tips: the quad set
 * up at the start fills four points, the last of which is the first tip. */
typedef union {
    struct {
        SVECTOR base[3];
        SVECTOR shards[32];
    } s;
    SVECTOR flat[35];
} Model762Shards;

/* The 0x714-byte work area: the command row, the sparks, the drift of the
 * rising wings, the fan centre and rim, the shards, the dots and their
 * velocities, the inner and outer ring points, the projected dots, the
 * textures, the ambient level, the wing scale, the ring pulse, the flash
 * size, the phase, the wing state, the elapsed time, the dot depths and the
 * colours. */
typedef struct {
    Model762Config *G32 config;
    SVECTOR sparks[32];
    SVECTOR drift;
    SVECTOR fan[32];
    Model762Shards shard;
    SVECTOR dots[32];
    SVECTOR dot_velocities[32];
    SVECTOR inner[16];
    SVECTOR outer[16];
    DVECTOR screen[32];
    s32 texture[4];
    s32 ambient;
    s32 wing_scale;
    s32 pulse;
    s16 flash;
    s16 phase;
    s16 state;
    s16 elapsed;
    u16 depths[32];
    Model762Colors colors;
} Model762State;

extern GsIMAGE gModelVariant762_D1F5C[4];
extern Model762Config gModelVariant762_D1FCC[];
extern SVECTOR gModelVariant762_D1FFC;

s32 ModelVariant762_HandleRequest(u8 *context, s32 command);

#endif
