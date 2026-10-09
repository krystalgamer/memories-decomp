#ifndef FRENCH_MODEL166_414C_CANDIDATE8_H
#define FRENCH_MODEL166_414C_CANDIDATE8_H

typedef struct {
    SVECTOR rows[3][17];
    CVECTOR colors[17];
    s32 progress;
    s32 completed;
} Model166Ring;

typedef struct {
    Model166Ring rings[3];
    u8 unknown_5AC[0x2028];
    POLY_GT4 quad;
    u8 unknown_2608[0x88];
    s32 translation[3];
    SVECTOR target;
    s32 direction[3];
    u8 unknown_26B0[0x20];
    s32 frame;
    u8 unknown_26D4[8];
    s32 step;
    u8 unknown_26E0[0x3C];
    s32 phase;
} Model166State;

#endif
