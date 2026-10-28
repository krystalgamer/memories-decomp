#ifndef MEMORIES_DECOMP_MODEL_VARIANT843_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT843_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x14-byte command row: the orb and the comet colours, the two model
 * parts the orbs and comets leave from, the four parts the sparks may leave
 * from, the orb spread, the spark switch, and the timing. */
typedef struct {
    u8 orb_r, orb_g, orb_b;
    u8 comet_r, comet_g, comet_b;
    u8 parts[2];
    u8 spark_parts[4];
    u16 spread;
    u16 sparks;
    u16 start;
    u16 emit_end;
} Model843Config;

/* The work area, one half per model part: the orbs with their velocities
 * and targets, the comets with their velocities and targets, the sparks,
 * the orb spins, the timers, the spark frames, the counts, the phase, the
 * arrival flags, the next spark, the ambient level, the burst texture page
 * and palette, the textures, and the fading orb and comet colours. */
typedef struct {
    Model843Config *G32 config;
    SVECTOR orbs[2][96];
    SVECTOR orb_velocities[2][96];
    SVECTOR orb_targets[2][96];
    SVECTOR comets[2][32];
    SVECTOR comet_velocities[2][32];
    SVECTOR comet_targets[2][32];
    SVECTOR sparks[32];
    u16 orb_spins[2][96];
    u32 elapsed;
    u32 tick;
    u16 spark_frames[32];
    u16 orb_count;
    u16 comet_count;
    u16 phase;
    u16 orb_arrived[2][96];
    u16 comet_arrived[2][32];
    u16 spark_index;
    s32 level;
    u16 tpage, clut;
    s32 texture[2];
    u8 orb_colors[2][96][4];
    u8 comet_colors[2][32][4];
} Model843State;

extern GsIMAGE D_8013CC90[2];
extern Model843Config D_8013CCE4[];

s32 func_8013B004(u8 *context, s32 command);

#endif
