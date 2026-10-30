#ifndef MEMORIES_DECOMP_MODEL_VARIANT759_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT759_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_get_slot_dimensions_with_minimums.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x28-byte command row: the glow colour, the first and the second
 * wave colours, the glow, wave length, flash and wave height sizes, the
 * wave depth offset, the two wave tilts, the ring height and the start,
 * second-pass, first-pass, hold and end times. */
typedef struct {
    u8 r, g, b;
    u8 first[3];
    u8 second[3];
    u8 unknown_9;
    u16 glow_size;
    u16 wave_length;
    u16 flash_size;
    s16 depth;
    u16 wave_height;
    s32 first_tilt;
    s32 second_tilt;
    u16 ring_height;
    s16 start;
    s16 second_time;
    s16 first_time;
    s16 hold;
    s16 end;
} Model759Config;

/* The work area: the command row, the glow, the two wave and the flash
 * quads, the near and far ring points, the ambient level, the textures, the
 * wave colours per pass and channel, the ring brightness, the per-channel
 * fade-done flags, the glow and flash frames, the phase, the elapsed time
 * and the ring count. */
typedef struct {
    Model759Config *G32 config;
    SVECTOR glow[4];
    SVECTOR wave[4];
    SVECTOR wave2[4];
    SVECTOR flash[4];
    /* The nine ring points seen either as SVECTORs or as their four s16
     * words; the ring setup writes vy and vz through the word view. */
    union {
        SVECTOR v[9];
        s16 c[9][4];
    } near_ring, far_ring;
    s32 ambient;
    s32 texture[3];
    u8 colors[2][3][2];
    u8 ring_alpha;
    u8 unknown_131;
    s16 done[2][2][3];
    s16 glow_frame[2];
    s16 flash_frame[2];
    s16 phase;
    s16 elapsed;
    s16 ring_count;
} Model759State;

extern GsIMAGE D_8013CDE4[3];
extern Model759Config D_8013CE38[];

s32 func_8013B004(u8 *context, s32 command);

#endif
