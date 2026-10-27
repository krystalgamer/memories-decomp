#ifndef MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT177_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/sdk_internal.h"

/* One 0x14-byte command row, picked by command % 100: the beam and particle
 * colours, a part byte this entry does not read, the ring radius, the particle
 * half size, the spin rate, the fade, the duration and the delay. */
typedef struct {
    u8 beam_r, beam_g, beam_b;
    u8 particle_r, particle_g, particle_b;
    u8 part, unknown_07;
    s16 radius, particle_half_size, spin, fade, duration, delay;
} Model177Config;

/* The 0x18C-byte work area: the command row, the fourteen ring points, the
 * origin next to the other slot, 32 particles (the padding halfword is the
 * particle's animation frame, or -1 once it is spent), two texture words, the
 * elapsed time, the command group (command / 100) and the completion flag. */
typedef struct {
    Model177Config *G32 config;
    SVECTOR ring[14];
    SVECTOR origin;
    SVECTOR particles[32];
    s32 texture[2];
    s32 elapsed;
    u8 command_group, completed, unknown_18A[2];
} Model177State;

extern GsIMAGE D_8013C510[];
extern Model177Config D_8013C548[];

s32 func_8013B004(u8 *context, s32 command);

#endif
