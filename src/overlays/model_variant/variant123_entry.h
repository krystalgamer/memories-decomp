#ifndef MEMORIES_DECOMP_MODEL_VARIANT123_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT123_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_queries.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x2E-byte command row: the moving quad and the ring colours, the slot
 * part the quads start on, the quad half size, the travel distance and the
 * extra push toward the other slot, the ring's radius and width and height,
 * the move duration, the ring span, ring duration and flash duration, the
 * spacing between quads and between rings, the quad count, and the ring,
 * move and flash delays. */
typedef struct {
    u8 red, green, blue, ring_red, ring_green, ring_blue, part, unknown_07;
    s16 half_size, unknown_0A, distance, push, unknown_10, radius, width, ring_y;
    s16 duration, ring_span, ring_duration, flash_duration, spacing, unknown_22, ring_spacing, count;
    s16 ring_delay, move_delay, flash_delay;
} Model123Config;

/* The 0x434-byte work area: the command row, up to 96 moving quad positions,
 * the other slot's position, the 34 ring points, this slot's position, the two
 * textures, the elapsed time and the completion flag. */
typedef struct {
    Model123Config *G32 config;
    SVECTOR points[96];
    SVECTOR other;
    SVECTOR ring[34];
    SVECTOR self;
    s32 texture[2];
    s32 elapsed;
    u8 completed, unknown_431[3];
} Model123State;

extern GsIMAGE D_8013C350[];
extern Model123Config D_8013C388[];

s32 func_8013B004(u8 *context, s32 command);

#endif
