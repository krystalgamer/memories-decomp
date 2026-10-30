#ifndef FRENCH_MODEL_IMAGE12048_CHAINS_H
#define FRENCH_MODEL_IMAGE12048_CHAINS_H

#include "../../types.h"
#include "../model_variant/model_variant.h"

typedef struct {
    /* pos, sxy, angle and edge are contiguous: 27 SVECTOR-sized slots, and
       edge[j] sits 18 slots after pos[j]. span is that whole region. */
    union {
        struct {
            SVECTOR pos[9];
            PSXLONG sxy[9];
            s32 angle[9];
            SVECTOR edge[9];
        } f;
        SVECTOR span[27];
    } u;
    PSXLONG edge_sxy[9];
    s32 width[9];
    u8 unknown_120[0x24];
    CVECTOR color[9];
    s32 stage;
    s32 dir[3];
    u8 unknown_178[4];
    s32 start;
    s32 end;
    s32 yaw;
    s32 pitch;
    s32 wave;
    s32 sway;
    s32 depth[9];
    s16 dx[9];
    s16 dy[9];
} Chains12048Chain;

typedef struct {
    u8 unknown_000[0xA78];
    Chains12048Chain chains[3];
    u8 unknown_100C[0xA8];
    POLY_FT4 polys[2];
    u8 unknown_1104[0xD0];
    SVECTOR offset;
    u8 unknown_11DC[0x158];
    s32 target[3];
    u8 unknown_1340[0x80];
    s32 phase;
} Chains12048State;

#endif
