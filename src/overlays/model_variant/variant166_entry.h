#ifndef MEMORIES_DECOMP_MODEL_VARIANT166_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT166_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x24-byte command row, picked by command % 100: the particle colour,
 * three glow colour bytes this entry does not read, the slot part the
 * particles start from, the particle count, the particle half size, the
 * target sphere radius, the glow half size, the ring radius, the rise, fade
 * and pulse durations, the gather and burst durations, the spacing between
 * particles, a spin rate this entry does not read and the three start
 * times. */
typedef struct {
    u8 red, green, blue, glow_red, glow_green, glow_blue, part, count;
    s16 size, radius, glow_size, ring_radius, rise, fade, pulse, gather, burst, spacing, spin;
    s16 delay, delay2, start;
} Model166Config;

/* The 0x63C-byte work area: the command row, 64 particle positions and 64
 * target points, the gather centre, a 66-point double ring, five texture
 * words, the completion flag, the elapsed time and a frame count. */
typedef struct {
    Model166Config *G32 config;
    SVECTOR positions[64];
    SVECTOR targets[64];
    SVECTOR centre;
    SVECTOR ring[66];
    u32 texture[5];
    u8 completed, unknown_631[3];
    s32 elapsed;
    s32 frames;
} Model166State;

extern GsIMAGE gModelVariant166_D1CDC[];
extern Model166Config gModelVariant166_D1D68[];

s32 ModelVariant166_HandleRequest(u8 *context, s32 command);

#endif
