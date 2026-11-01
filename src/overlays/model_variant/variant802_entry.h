#ifndef MEMORIES_DECOMP_MODEL_VARIANT802_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT802_ENTRY_H

#include "../../types.h"
#include "../../game/screen_projection.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/model_update_coordinate_unit_matrix.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"

/* One 0x28-byte command row: the particle size and count, the rise speed,
 * the cloud spread, the number of approach frames, the slot scale step, and
 * the approach, hold, grow, settle, shrink and return times. */
typedef struct {
    u16 size;
    u16 count;
    u16 speed;
    u16 unknown_06;
    u16 spread;
    u16 frames;
    u32 scale_step;
    u32 approach_end;
    u32 hold_end;
    u32 grow_end;
    u32 settle_end;
    u32 return_end;
    u32 unknown_24;
} Model802Config;

/* The work area: the command row, the slot offset and its per-frame
 * velocity, the particle positions, the slot scale, the elapsed time, the
 * phase, the live particle count, the particle rise speeds and ages, the
 * ambient level and the particle texture. */
typedef struct {
    Model802Config *G32 config;
    VECTOR offset;
    VECTOR velocity;
    SVECTOR particles[128];
    u32 scale;
    u32 elapsed;
    u16 phase;
    u16 count;
    u16 speeds[128];
    u16 ages[128];
    s32 level;
    s32 texture[1];
} Model802State;

extern GsIMAGE gModelVariant802_D12DC[];
extern Model802Config gModelVariant802_D12F8[];

s32 ModelVariant802_HandleRequest(u8 *context, s32 command);

#endif
