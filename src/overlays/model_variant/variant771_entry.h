#ifndef MEMORIES_DECOMP_MODEL_VARIANT771_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT771_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x2C-byte command row: the colour, the four model parts the trails
 * follow, the forward drift of the fan, the fan height, the flare quad's
 * height and depth, the inner and outer ring radii, the ring growth before
 * and after the threshold, the cone height, the trail start and end, the end
 * of the effect and the two camera-shake times. */
typedef struct {
    u8 r, g, b;
    u8 parts[4];
    u8 unknown_07;
    s16 drift;
    s16 fan_height;
    s16 quad_height;
    s16 quad_depth;
    s16 inner_radius;
    s16 outer_radius;
    s32 grow;
    s32 grow_late;
    s16 cone_height;
    s16 trail_start;
    s16 trail_end;
    s16 unknown_22;
    s32 end;
    u16 shake[2];
} Model771Config;

/* The fan colour, tested as a 24-bit word to know whether it faded out,
 * followed by the per-segment trail brightness and the two ring greys. */
typedef union {
    struct {
        u8 r, g, b;
        u8 fade[64];
        u8 grey[2];
    } c;
    u32 word;
} Model771Tint;

/* The 0xAAC-byte work area: the command row, the four trails, the fan
 * offset, the two fan edges, the flare quad, an unused vector, the inner,
 * outer and cone rings, the ambient level, the textures, the phase, the shake
 * count, the elapsed time, the ring scale, the trail head, the frame count
 * and the colours. */
typedef struct {
    Model771Config *G32 config;
    SVECTOR trail0[64];
    SVECTOR trail1[64];
    SVECTOR trail2[64];
    SVECTOR trail3[64];
    SVECTOR offset;
    SVECTOR fan_a[9];
    SVECTOR fan_b[9];
    SVECTOR quad[4];
    SVECTOR unknown_8BC;
    SVECTOR inner[16];
    SVECTOR outer[16];
    SVECTOR cone[16];
    s32 ambient;
    s32 texture[3];
    s16 phase;
    u16 shakes;
    s32 elapsed;
    s32 scale;
    s16 head;
    s16 frame;
    Model771Tint tint;
} Model771State;

extern GsIMAGE D_8013CFB0[3];
extern Model771Config D_8013D004[];

s32 func_8013B004(u8 *context, s32 command);

#endif
