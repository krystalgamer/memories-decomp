#ifndef MEMORIES_DECOMP_MODEL_VARIANT778_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT778_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x22-byte command row: the four model parts the smoke rises from, the
 * smoke puff size, the glow spread and size, the cloud radius, the ring size,
 * spread and height, the ring segment count, and the delay, glow start, end
 * and the window that pauses the smoke. */
typedef struct {
    u8 parts[4];
    u16 smoke_size;
    s16 spread;
    u16 unknown_08;
    u16 glow_size;
    s16 radius;
    u16 unknown_0E;
    u16 ring_size;
    s16 ring_spread;
    u16 ring_height;
    s16 segments;
    s16 delay;
    s16 glow_time;
    s16 end;
    u16 pause_start;
    u16 pause_end;
} Model778Config;

/* The work area: the command row, the smoke puffs, the glows, the ring
 * centres, the ring segment tips, the two ring axis ends, the ring scales,
 * the smoke and glow ages, the elapsed time, the frame, the next smoke puff,
 * the glow and ring counts, the ambient level, the phase, the textures and
 * the ring brightness. */
typedef struct {
    Model778Config *G32 config;
    SVECTOR smoke[25];
    SVECTOR glows[128];
    SVECTOR rings[32];
    SVECTOR tips[32][32];
    SVECTOR axis[2];
    s32 scales[32];
    u16 smoke_ages[25];
    s16 glow_ages[128];
    s16 elapsed;
    u16 frame;
    s16 head;
    s16 glow_count;
    s16 ring_count;
    s32 level;
    s32 phase;
    s32 texture[3];
    u8 alpha[32];
} Model778State;

extern GsIMAGE D_8013C4C4[];
extern Model778Config D_8013C518[];

s32 func_8013B004(u8 *context, s32 command);

#endif
