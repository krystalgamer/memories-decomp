#ifndef FRENCH_MODEL_VARIANT87_ENTRY_H
#define FRENCH_MODEL_VARIANT87_ENTRY_H

#include "../../types.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_slot_data.h"
#include "../../game/model_slot_queries.h"
#include "../../game/gpu_packets.h"
#include "../../game/func_80058E1C.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/memory.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 outer[3];
    u8 inner[3];
    u8 part;
    u8 count;
    s16 radius;
    s16 spread;
    s16 growth;
    s16 hold;
    s16 fade;
    s16 interval;
    s16 delay;
} Family87Config;

typedef struct {
    Family87Config *G32 config;
    SVECTOR points[26];
    /* Measured offset span, not an allocation-capacity declaration. */
    u8 destination_storage[0x100];
    s32 frame;
    u8 completed;
} Family87State;

extern Family87Config D_8013BA1C[];
s32 func_8013B004(u8 *context, s32 command);

#endif
