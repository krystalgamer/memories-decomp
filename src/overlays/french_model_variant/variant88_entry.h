#ifndef FRENCH_MODEL_VARIANT88_ENTRY_H
#define FRENCH_MODEL_VARIANT88_ENTRY_H
#include "../../types.h"
#include "../../game/model_control.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"

typedef struct {
    u8 r, g, b, groups, count, parts[5];
    s16 size, amplitude, period, delay, duration;
    u8 r1, g1, b1, pad;
} Family88Config;

typedef struct {
    Family88Config *G32 config;
    /* Offset spans, not a claim about backing allocation capacity. */
    SVECTOR points[512];
    s16 timers[512];
    s32 frame;
    union {
        u32 textures[2];
        struct {
            u32 texture;
            u8 done;
            u8 padding[3];
        } state;
    } shared;
} Family88State;

extern u8 D_8013B954[];
s32 func_8013B004(u8 *context, s32 command);
#endif
