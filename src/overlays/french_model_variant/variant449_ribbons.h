#ifndef FRENCH449_RIBBONS_VIEW_H
#define FRENCH449_RIBBONS_VIEW_H

#include "../../types.h"
#include "variant449_sheet.h"

typedef union {
    DVECTOR point;
    PSXLONG packed;
} Variant449Screen;

typedef struct {
    SVECTOR a[17];
    Variant449Screen sa[17];
    s32 angle[17];
    SVECTOR b[17];
    Variant449Screen sb[17];
    s32 width[17];
    u8 color[4];
    u8 unknown_224[0xA4];
    VECTOR position;
    VECTOR delta;
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
    s32 start;
    s32 end;
    s32 state;
    s32 count;
} Variant449Ribbon;

typedef struct {
    Variant449Ribbon ribbons[4];
    u8 unknown_0E00[0xCA8];
    POLY_FT4 packets[2];
    u8 unknown_1AF8[0x5C];
    s32 center[3];
    u8 unknown_1B60[0x20];
    VECTOR direction;
    u8 unknown_1B90[0x34];
    s32 axis_x;
    s32 axis_y;
    s32 axis_z;
    u8 unknown_1BD0[0x10];
    s32 elapsed;
    u8 unknown_1BE4[4];
    s32 step;
    u8 unknown_1BEC[8];
    Variant449Timing *G32 timing;
    u8 unknown_1BF8[0x24];
    s32 spin;
    u8 unknown_1C20[0x14];
    s32 ripple;
    s32 ripple2;
} Variant449RibbonView;

void func_8013C240(u8 *context);
#endif
