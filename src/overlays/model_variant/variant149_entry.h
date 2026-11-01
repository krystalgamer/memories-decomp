#ifndef MEMORIES_DECOMP_MODEL_VARIANT149_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT149_ENTRY_H

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
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x28-byte command row, picked by command % 100: the orb colour, the
 * ring colour, three unused bytes, the slot part the orbs gather on, the orb
 * half size, the spread of the orb targets, the outer and inner ring radii,
 * the ring depth, the burst half size, the flight, ring and burst durations,
 * the orb spin rate and the start time of each of the five orbs. */
typedef struct {
    u8 red, green, blue, ring_red, ring_green, ring_blue, unknown_6[3], part;
    s16 size, spread, outer, inner, depth, burst_size, flight, ring, burst, spin, delay[5];
} Model149Config;

/* The 0x24C-byte work area: the command row, five orb positions (the pad
 * holds the roll), five targets and the ring rotations aimed at them, a
 * 51-point ring mesh, the orb animation frames, the texture words, the
 * completion counter and the elapsed time. */
typedef struct {
    Model149Config *G32 config;
    SVECTOR orbs[5];
    SVECTOR targets[5];
    SVECTOR rotations[5];
    SVECTOR ring[51];
    u8 frames[5], unknown_219[3];
    u32 texture[10];
    u8 completed, unknown_245[3];
    s32 elapsed;
} Model149State;

extern GsIMAGE gModelVariant149_D14D4[];
extern Model149Config gModelVariant149_D15EC[];

s32 ModelVariant149_HandleRequest(u8 *context, s32 command);

#endif
