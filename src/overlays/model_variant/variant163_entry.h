#ifndef MEMORIES_DECOMP_MODEL_VARIANT163_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT163_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x24-byte command row: the disc, tile and ring colours, the slot part
 * the effect grows from, the disc radius, the tile size, the inner and outer
 * ring radii, the grow, flight, flash, push, hold and ring durations, the
 * spacing between tiles and the start delay. */
typedef struct {
    u8 red, green, blue;
    u8 tile_red, tile_green, tile_blue;
    u8 ring_red, ring_green, ring_blue;
    u8 part;
    s16 unknown_A;
    s16 radius;
    s16 size;
    s16 inner;
    s16 outer;
    s16 grow;
    s16 fly;
    s16 flash;
    s16 push;
    s16 hold;
    s16 ring;
    s16 spacing;
    s16 delay;
} Model163Config;

/* The 0x274-byte work area: the command row, eight tile positions, a
 * 34-point disc, a 3x5 tile grid, the grid offset, an 18-point double ring,
 * two texture words, the completion flag and the elapsed time (negative
 * during the start delay). */
typedef struct {
    Model163Config *G32 config;
    SVECTOR positions[8];
    SVECTOR disc[34];
    SVECTOR grid[15];
    SVECTOR offset;
    SVECTOR rings[18];
    s32 texture[2];
    u8 completed, unknown_26D[3];
    s32 elapsed;
} Model163State;

extern GsIMAGE gModelVariant163_D2414[];
/* The atlas cell drawn by each tile. */
typedef struct {
    u8 cell[8];
} Model163Order;

extern Model163Config gModelVariant163_D244C[];

s32 ModelVariant163_HandleRequest(u8 *context, s32 command);

#endif
