#ifndef MODEL_VARIANT122_ENTRY_H
#define MODEL_VARIANT122_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_properties.h"
#include "../../game/model_get_frame_step.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 glow_red, glow_green, glow_blue;
    u8 fan_red, fan_green, fan_blue;
    u8 flash_red, flash_green, flash_blue;
    u8 spray_red, spray_green, spray_blue;
    u8 dust_level;
    u8 unknown_0D;
    u8 unknown_0E;
    u8 slot;
    s16 radius;
    s16 fan_size;
    s16 height;
    s16 spray_size;
    s16 spray_spread;
    s16 dust_spread;
    s16 life;
    s16 flash_time;
    s16 spray_time;
    s16 dust_time;
    s16 spray_interval;
    s16 start;
    s16 release;
    s16 spray_start;
} Model122Command;

typedef struct {
    Model122Command *G32 command;
    SVECTOR particles[64];
    SVECTOR spray[32];
    SVECTOR dust[16];
    s16 life[64];
    u32 texture[2];
    u8 done;
    s32 elapsed;
} Model122State;

extern VECTOR D_8013C840;
extern GsIMAGE D_8013C850[];
extern Model122Command D_8013C888[];

#endif
