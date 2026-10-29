#ifndef MEMORIES_DECOMP_MODEL_VARIANT105_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT105_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../game/screen_projection.h"

/* One 0x1C-byte command row: the sprite colour, the slot part the shots
 * leave from, the launch sprite size, the launch offset, the target spread,
 * the arc height, the burst sprite size, the fade-in, travel and burst
 * times, the launch and burst spacings, the start delay and the launch
 * time. */
typedef struct {
    u8 red, green, blue;
    u8 part;
    s16 size;
    s16 offset;
    s16 spread;
    s16 arc;
    s16 burst_size;
    s16 fade;
    s16 travel;
    s16 burst;
    s16 spacing;
    s16 burst_spacing;
    s16 delay;
    s16 launch;
} Model105Config;

/* The trail and target positions (0x44..0x144): each shot's three trail
 * positions followed by the eight targets. The native walks step through the
 * trails three at a time from the current trail index, so the last stride
 * reaches into the targets; flat is the whole 32-entry region they walk. */
typedef union {
    struct {
        SVECTOR trails[8][3];
        SVECTOR targets[8];
    } s;
    SVECTOR flat[32];
} Model105Shots;

/* The texture words (0x144..0x14C): the setup loop writes two packed
 * page/clut words, the second over the current trail index and its padding
 * (the index is cleared first, so after setup it holds the low byte of the
 * second word; the draw paths only use the first word). */
typedef union {
    u32 words[2];
    struct {
        u32 texture;
        u8 index;
        u8 unknown_149[3];
    } s;
} Model105Texture;

/* The 0x150-byte work area: the command row, each shot's spin, its trail
 * and target positions, the texture words with the current trail index and
 * the elapsed time (negative during the start delay). */
typedef struct {
    Model105Config *G32 config;
    SVECTOR spins[8];
    Model105Shots shots;
    Model105Texture texture;
    s32 elapsed;
} Model105State;

extern GsIMAGE D_8013C424[];
extern Model105Config D_8013C440[];

s32 func_8013B004(u8 *context, s32 command);

#endif
