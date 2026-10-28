#ifndef FRENCH_MODEL_IMAGE16740_ENTRY_H
#define FRENCH_MODEL_IMAGE16740_ENTRY_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../psyq/string.h"
#include "../../game/model_graphics_state.h"
#include "../../game/model_copy_slot_position.h"
#include "../../game/model_texture_upload.h"
#include "../../game/screen_projection.h"
#include "../../game/graphics_packet_submission.h"

typedef struct {
    u8 beam_color[3];
    u8 ring_color[3];
    u8 unknown_06[2];
    s16 height;
    s16 half_width;
    s16 spread;
    s16 radius;
    s16 thickness;
    s16 ring_height;
    s16 beam_duration;
    s16 ring_span;
    s16 ring_duration;
    s16 flash_duration;
    s16 beam_spacing;
    s16 ring_spacing;
    s16 ring_begin;
    s16 beam_begin;
    s16 flash_begin;
} Image16740Config;

typedef struct {
    Image16740Config *G32 config;
    SVECTOR beams[32];
    SVECTOR origin;
    SVECTOR ring[66];
    SVECTOR translation;
    s32 textures[2];
    s32 time;
    u8 completed;
} Image16740State;

extern VECTOR D_8013BC44;
extern GsIMAGE D_8013BC54[2];
extern Image16740Config D_8013BC8C[];

#endif
