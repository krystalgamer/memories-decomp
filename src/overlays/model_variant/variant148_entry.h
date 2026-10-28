#ifndef MEMORIES_DECOMP_MODEL_VARIANT148_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT148_ENTRY_H

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

/* One 0x1C-byte command row, picked by command % 100: the colour, the slot
 * part the shells start from, the shell length and radius, the target ring
 * radius, the spark half size, the spark sphere radius, the spacing between
 * shells, the flight and spark durations, the flash duration, the roll and
 * spin rates and the start time. */
typedef struct {
    u8 red, green, blue, part;
    s16 size, radius, spread, spark_size, burst, spacing, rise, fade, flash, roll, spin, delay;
} Model148Config;

/* The 0x384-byte work area: the command row, a 30-point shell mesh, 24 shell
 * positions and 24 targets, 32 spark offsets, the texture word, the
 * completion and flash flags and the elapsed time. */
typedef struct {
    Model148Config *G32 config;
    SVECTOR points[30];
    SVECTOR positions[24];
    SVECTOR targets[24];
    SVECTOR sparks[32];
    u32 texture[1];
    u32 unknown_378;
    u8 completed, flash, unknown_37E[2];
    s32 elapsed;
} Model148State;

extern GsIMAGE D_8013C728[];
extern Model148Config D_8013C744[];

s32 func_8013B004(u8 *context, s32 command);

#endif
