#ifndef MEMORIES_DECOMP_MODEL_VARIANT787_ENTRY_H
#define MEMORIES_DECOMP_MODEL_VARIANT787_ENTRY_H

#include "../../types.h"
#include "../../game/screen_projection.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_state_setters.h"
#include "../../game/model_texture_upload.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_effect_state.h"
#include "../../game/gpu_packets.h"
#include "../../game/graphics_packet_submission.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/string.h"

/* One 0x2C-byte command row: the band and glow colours, the band and glow
 * sizes, the camera spin, the spoke tips and their depth, the spoke count,
 * the band and glow fade steps, and the start and camera-switch times. */
typedef struct {
    u8 band_r, band_g, band_b;
    u8 glow_r, glow_g, glow_b;
    s16 band_depth;
    s16 band_height;
    s16 unknown_0A;
    s32 spin;
    s16 glow_size;
    s16 tip_depth;
    s16 spoke_length;
    s16 spoke_count;
    s16 spoke_depth;
    s16 unknown_1A;
    u8 band_fade, unknown_1D;
    u8 glow_fade, unknown_1F;
    s32 unknown_20;
    s32 unknown_24;
    s16 delay;
    s16 switch_time;
} Model787Config;

/* The work area: the command row, the two spoke tips, the band stage, the
 * elapsed time, the phase, the ambient level, the textures, the band
 * colours, the glow colour, the flash and spoke brightness and the spoke
 * colour. */
typedef struct {
    Model787Config *G32 config;
    SVECTOR tips[2];
    u16 stage;
    u32 elapsed;
    u16 phase;
    s32 level;
    s32 texture[3];
    u8 red[3];
    u8 green[3];
    u8 blue[3];
    u8 glow_r, glow_g, glow_b;
    u8 flash;
    u8 spoke;
    u8 spoke_r, spoke_g, spoke_b;
} Model787State;

extern GsIMAGE gModelVariant787_D12EC[];
extern Model787Config gModelVariant787_D1324[];

s32 ModelVariant787_HandleRequest(u8 *context, s32 command);

#endif
