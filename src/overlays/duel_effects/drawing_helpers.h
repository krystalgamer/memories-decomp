#ifndef MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_HELPERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_DRAWING_HELPERS_H

#include "../../types.h"
#include "utility_helpers.h"

extern RECT D_8015B3C0[12];
extern GsOT *G32 D_8015B7F4;
extern SVECTOR D_8015B7F8;
extern u16 D_8015B800;
/* Error texts func_8014E35C prints in the NTSC banks; the PAL banks have none. */
extern char gNorthAmerican_D_80146014[];
extern char gNorthAmerican_D_8014602C[];

void func_8014E35C(s32 mode);
void func_8015131C(POLY_GT4 *packet, SVECTOR *vertices, s16 bias, u16 mode);
void func_80152EC4(void *primitive, u16 flags);
void func_80152F9C(POLY_FT4 *packet, u16 mode);
void func_801530B0(POLY_GT4 *packet, u16 mode);
void func_80156448(u8 *color, SVECTOR *first, SVECTOR *middle, SVECTOR *last, u16 bias);
void func_801566D4(s32 value, u8 *color, SVECTOR *offset, u16 mode,
                   u16 size, u16 bias);
u16 func_80156AD4(s16 value);
void func_80156B40(u16 value, u16 *digits);
void func_80156E58(u8 *color, u16 width, SVECTOR *positions, u16 count, s16 bias);
void func_80156FA4(u8 *color, SVECTOR *vertices, u16 flags, u16 mode);
u16 func_801570B0(u8 *color, u16 count, SVECTOR *vertices, u16 bias,
                    u16 mode, SVECTOR *output, u16 flags, SVECTOR *offset);

#endif
