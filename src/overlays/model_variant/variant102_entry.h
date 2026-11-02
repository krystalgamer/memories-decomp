#ifndef MEMORIES_DECOMP_MODEL_VARIANT102_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT102_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x28-byte command row, picked by the command: the shell colour, the
 * petal colour, the spark colour, the dome radius and height, the petal half
 * size, the target spread, the glow half size, the spark half size and
 * sphere radius, the dome rise and hold durations, the petal growth, flight
 * and spark durations, the spacing between petal rings and the start time. */
typedef struct {
    u8 red, green, blue, petal_red, petal_green, petal_blue, unknown_6[3];
    u8 spark_red, spark_green, spark_blue;
    s16 radius, height, size, spread, glow_size, spark_size, spark_radius;
    s16 rise, hold, grow, flight, burst, spacing, delay;
} Model102Config;

/* The 0xB08-byte work area: the command row, a 12-ring dome mesh of nine
 * points per ring plus its apex, 96 petal positions and 96 petal targets,
 * 48 spark offsets, five texture words, the completion flag and the elapsed
 * time. */
typedef struct {
    Model102Config *G32 config;
    SVECTOR mesh[109];
    SVECTOR petals[96];
    SVECTOR targets[96];
    SVECTOR sparks[48];
    s32 texture[5];
    u8 completed, unknown_B01[3];
    s32 elapsed;
} Model102State;

extern GsIMAGE gModelVariant102_D24CC[];
extern Model102Config gModelVariant102_D2558[];

s32 ModelVariant102_HandleRequest(u8 *context, s32 command);

#endif
