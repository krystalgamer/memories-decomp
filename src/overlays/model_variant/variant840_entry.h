#ifndef MEMORIES_DECOMP_MODEL_VARIANT840_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT840_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

/* One 10-byte command row: the four model parts the streak groups leave
 * from, the launch time, the stop time and one unused word. */
typedef struct {
    u8 part[4];
    u16 start;
    u16 stop;
    u16 unused;
} Model840Config;

/* The work area: the command row; per part and streak its position, step
 * and burst position; the timers; per part and streak its burst scale; the
 * phase; per part and streak its state; the streak count, the ambient level,
 * the burst texture, the two loaded textures and per part and streak its
 * alpha and burst colour. */
typedef struct {
    Model840Config *G32 config;
    SVECTOR pos[4][8];
    SVECTOR vel[4][8];
    SVECTOR burst_pos[4][8];
    u32 elapsed;
    u32 tick;
    u32 scale[4][8];
    u16 phase;
    u16 state[4][8];
    u16 count;
    s32 level;
    u16 burst_tpage;
    u16 burst_clut;
    s32 texture[2];
    u8 alpha[4][8];
    CVECTOR burst_color[4][8];
} Model840State;

extern GsIMAGE D_8013BEFC[];
extern Model840Config D_8013BF34[];

s32 func_8013B004(u8 *context, s32 command);

#endif
