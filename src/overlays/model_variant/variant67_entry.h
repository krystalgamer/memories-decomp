#ifndef MODEL_VARIANT67_ENTRY_H
#define MODEL_VARIANT67_ENTRY_H

#include "../../types.h"
#include "../../game/model_slot_getters.h"
#include "../../game/model_slot_support.h"
#include "../../game/model_texture_upload.h"
#include "../../game/gpu_packets.h"
#include "../../psyq/libhmd.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/rand.h"

typedef struct {
    u8 ribbon_red, ribbon_green, ribbon_blue;
    u8 edge_red, edge_green, edge_blue;
    u8 spark_red, spark_green, spark_blue;
    u8 flash_red, flash_green, flash_blue;
    s16 slot;
    s16 base_radius;
    s16 top_radius;
    s16 ring_radius;
    s16 spark_size;
    s16 flash_size;
    s16 height;
    s16 move_time;
    s16 wobble_speed;
    s16 spin_speed;
    s16 delay;
    s16 grow_end;
    s16 fade_start;
    s16 end;
    s16 open_time;
} Model67Command;

typedef struct {
    Model67Command *command;
    SVECTOR ribbon[8][16];
    SVECTOR ring[8];
    CVECTOR color[16];
    u32 texture[3];
    u8 done;
    u8 frame[16];
    s32 elapsed;
} Model67State;

extern GsIMAGE gModelVariant67_aImages[];
extern Model67Command gModelVariant67_aConfigs[];

#endif
