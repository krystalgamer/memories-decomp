#ifndef MEMORIES_DECOMP_MODEL_VARIANT173US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT173US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x1A-byte command row, picked by command % 100: the cube colour, the
 * inner and outer ring colours, one unused byte, the cube half size, the
 * inner and outer ring radii, the flight and ring durations, the delay
 * between cubes, the cube count and the start time. */
typedef struct {
    u8 red, green, blue, inner_red, inner_green, inner_blue;
    u8 outer_red, outer_green, outer_blue, unknown_9;
    s16 size, inner, outer, flight, ring, stagger, count, delay;
} Model173UsConfig;

/* The 0x6D4-byte work area: the command row, up to 128 cube positions, the
 * eight unit cube corners, a 34-point ring mesh, the 24 corner pointers of
 * the six cube faces, the per-cube masks of the drawn copies, the texture
 * words, the completion counter, the elapsed time and the frame count. */
typedef struct {
    Model173UsConfig *G32 config;
    SVECTOR cubes[128];
    SVECTOR corners[8];
    SVECTOR ring[34];
    SVECTOR *G32 faces[24];
    u16 masks[128];
    u32 texture[5];
    u8 completed, unknown_6C9[3];
    s32 elapsed;
    s32 frames;
} Model173UsState;

extern GsIMAGE D_8013C2F0[];
extern Model173UsConfig D_8013C37C[];

s32 func_8013B004(u8 *context, s32 command);

#endif
