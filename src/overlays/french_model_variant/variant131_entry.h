#ifndef FRENCH_MODEL_VARIANT131_ENTRY_H
#define FRENCH_MODEL_VARIANT131_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/screen_projection.h"
#include "../../game/model_texture_upload.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

typedef struct {
    u8 beam_r, beam_g, beam_b;
    u8 particle_r, particle_g, particle_b;
    u8 part, unknown_07;
    s16 radius, particle_half_size, spin, fade, duration, delay;
} Model131Config;

typedef struct {
    Model131Config *G32 config;
    SVECTOR ring[14];
    SVECTOR origin;
    SVECTOR particles[32];
    s32 texture[2];
    s32 elapsed;
    u8 command_group, completed, unknown_18A[2];
} Model131State;

extern GsIMAGE D_8013C5CC[];
extern Model131Config D_8013C604[];
s32 func_8013B004(u8 *context, s32 command);

#endif
