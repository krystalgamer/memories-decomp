#ifndef MEMORIES_DECOMP_MODEL_VARIANT143US_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT143US_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/screen_projection.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"
#include "../../psyq/rand.h"
#include "../../psyq/stdio.h"

/* One 0x3E-byte command row: the wave's base colour and swing, the ring,
 * spark and dome colours, the joints the ring and the sparks start from, the
 * wave grid's size, lift and amplitude, the ring quad's width, height and
 * spread, the spark size, the spark target radius, the dome radius and
 * height, the burst scatter, then the wave fade, ring, flight, dome and burst
 * durations, the spark interval and the wave, fade-out, ring and spark start
 * frames. */
typedef struct {
    u8 base[3], swing[3], ring_colour[3], spark_colour[3];
    u8 dome_inner[3], dome_outer[3];
    u8 ring_joint, spark_joint;
    s16 grid_size, grid_lift, wave_height, ring_width, ring_height, ring_spread;
    s16 spark_size, target_radius, dome_radius, dome_height, scatter;
    s16 wave_fade, ring_time, flight_time, dome_time, burst_time, interval;
    s16 wave_start, fade_start, ring_start, spark_start;
} Model143UsConfig;

/* One vertex colour. */
typedef struct {
    u8 red, green, blue, unknown_03;
} Model143UsColour;

/* The 0x17FC-byte work area: the command row, the 13x13 wave grid and its
 * colours, the 32 ring offsets, 96 spark positions and their targets, the
 * 55-vertex dome and its colours, 48 bursts of four sparks, the 48 dome
 * spins, four texture words, the completion flag and the elapsed time. */
typedef struct {
    Model143UsConfig *G32 config;
    SVECTOR grid[169];
    Model143UsColour colours[169];
    SVECTOR ring[32];
    SVECTOR sparks[96];
    SVECTOR targets[96];
    SVECTOR dome[55];
    Model143UsColour dome_colours[55];
    SVECTOR burst[192];
    s16 spin[48];
    s32 texture[4];
    u8 completed, unknown_17F5[3];
    s32 elapsed;
} Model143UsState;

extern GsIMAGE D_8013DCA0[];
extern Model143UsConfig D_8013DD10[];

s32 func_8013B004(u8 *context, s32 command);

#endif
