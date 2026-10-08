#ifndef FRENCH_MODEL_VARIANT458_STRANDS_H
#define FRENCH_MODEL_VARIANT458_STRANDS_H
#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../psyq/sdk_internal.h"
#include "../../game/gpu_packets.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Variant458Screen;

typedef struct {
    SVECTOR a[9];
    Variant458Screen sa[9];
    s32 angle[9];
    SVECTOR b[9];
    Variant458Screen sb[9];
    s32 width[9];
    CVECTOR color;
    u8 unknown_124[0x84];
    s32 depth[9];
    s16 ox[9];
    s16 oy[9];
    u8 unknown_1F0[4];
    s32 count;
    u8 unknown_1F8[8];
} Variant458Strand;

typedef struct {
    Variant458Strand strands[6];
    u8 unknown_0C00[0xFD8];
    POLY_FT4 quads[2];
    u8 unknown_1C28[0x118];
    VECTOR origins[6];
    u8 unknown_1DA0[0x68];
    s32 size;
    u8 unknown_1E0C[0x84];
    VECTOR directions[6];
    u8 unknown_1EF0[4];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1F00[0x18];
    s32 speed;
    u8 unknown_1F1C[0x40];
    s32 spin;
    u8 unknown_1F60[0x14];
    s32 phase_a;
    s32 phase_b;
    s32 state;
} Variant458StrandView;

void func_8013C150(u8 *ctx);
#endif
