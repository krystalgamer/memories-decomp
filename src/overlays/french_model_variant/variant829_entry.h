#ifndef FRENCH_MODEL_VARIANT829_ENTRY_H
#define FRENCH_MODEL_VARIANT829_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_texture_upload.h"
#include "../../game/func_80057E20.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/abs.h"

typedef struct {
    u16 square_half_size, rectangle_half_width, rectangle_half_height;
    u16 animated_half_width, animated_half_height, wave_amplitude;
    u16 travel_divisor, cloud_spread, cloud_half_width, cloud_half_height, unknown_14;
    u8 fade_step, unknown_17;
    u16 startup_delay;
} Model829Config;

typedef union {
    struct {
        u16 phase, states[4], active;
    } fields;
    u32 first_pair;
} Model829Status;

typedef union {
    SVECTOR point;
    ModelEffectAdjustment adjustment;
} Model829Position;

typedef struct {
    Model829Config *G32 config;
    SVECTOR positions[4], velocities[4], cloud_offsets[4][32];
    u32 elapsed, updates, phase_frames;
    Model829Status status;
    u16 cloud_counts[4];
    s32 brightness, texture[4];
    u8 red[4], green[4], blue[4], cloud_gray[4][32];
} Model829State;

extern GsIMAGE D_8013BF98[];
extern Model829Config D_8013C008[];
s32 func_8013B004(u8 *context, s32 command);

#endif
