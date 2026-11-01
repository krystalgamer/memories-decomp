#ifndef MEMORIES_DECOMP_MODEL_VARIANT152_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT152_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/stdio.h"

/* One 0x1E-byte command row, picked by command % 100: the ribbon colours of
 * the two slots, the spark colours of the two slots, the slot data entries
 * the ribbons start from, the ring radius, the swirl radius, the spark half
 * size, the ribbon lifetime, the ribbon spacing and the start delay. */
typedef struct {
    /* Four RGBX colours, walked as one flat run. */
    u8 color[16];
    u8 entry[2];
    s16 ring_radius, swirl_radius, spark_size, duration;
    u8 spacing, unknown_1b;
    s16 delay;
} Model152Config;

/* The 0x470-byte work area: the command row, a 17-point ring, two 30-point
 * swirl targets, two 30-point ribbons, the two ribbon origins, two texture
 * words, the completion flag, the elapsed time and the frame count. */
typedef struct {
    Model152Config *G32 config;
    /* The 17-point ring followed by the two 30-point swirl targets; one
     * array because the spark cursor steps past the ring into the targets. */
    SVECTOR points[77];
    SVECTOR ribbon[60];
    SVECTOR origin[2];
    u32 texture[2];
    u8 completed, unknown_465[3];
    s32 elapsed;
    s32 frames;
} Model152State;

extern GsIMAGE gModelVariant152_D1158[];
extern Model152Config gModelVariant152_D1190[];

s32 ModelVariant152_HandleRequest(u8 *context, s32 command);

#endif
