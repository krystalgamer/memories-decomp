#ifndef MEMORIES_DECOMP_MODEL_VARIANT155_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT155_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_copy_slot_position.h"
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

/* One 0x2A-byte command row, picked by command % 100: the card, spark,
 * cross and dust colours, two unused bytes, the card half size (also the
 * spark spread), the orbit radius, height and lift, the cross radius, the
 * dust rise per pass, the total dust rise, the orbit period, the flash and
 * dust durations, the spacing between cards, the cross spin rate, the start
 * time and the end time. */
typedef struct {
    u8 red, green, blue, spark_red, spark_green, spark_blue;
    u8 cross_red, cross_green, cross_blue, dust_red, dust_green, dust_blue, unknown_C[2];
    s16 size, radius, height, lift, cross, dust_step, dust_rise, period, flash, dust, spacing, spin, delay, end;
} Model155Config;

/* The 0xE34-byte work area: the command row, a 256-point orbit path, 16
 * spark offsets per card, a 3x3 cross grid with its four quads, 96 dust
 * points, the texture words, the completion flag, the orbit phase of each
 * card, the elapsed time and the frame counter. */
typedef struct {
    Model155Config *G32 config;
    SVECTOR path[256];
    SVECTOR sparks[80];
    SVECTOR grid[9];
    SVECTOR dust[96];
    SVECTOR *G32 quads[4][4];
    u32 texture[5];
    u8 completed, unknown_E21;
    s16 phase[5];
    s32 elapsed;
    s32 frame;
} Model155State;

extern GsIMAGE D_8013C770[];
extern Model155Config D_8013C7FC[];

s32 func_8013B004(u8 *context, s32 command);

#endif
