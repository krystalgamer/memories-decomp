#ifndef MEMORIES_DECOMP_DUEL_EFFECT_UTILITY_HELPERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_UTILITY_HELPERS_H

#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"

s32 func_8014D378(const u8 *color);
s32 func_8014D3AC(const u8 *color);
void func_8014EA7C(u16 radius, SVECTOR *vertices);
void func_8014EC8C(u16 width, u16 depth, s32 level, s32 offset,
                   SVECTOR *vertices, u16 count);
void func_8014EB1C(u16 width, u16 height, SVECTOR *vertices, u16 count);
void func_8014EE0C(u16 width, u16 depth, s16 height, SVECTOR *vertices, u16 count);
void func_8014EF2C(u16 count, SVECTOR *vertices);
void func_8014F010(u8 *color, u8 value);
void func_8014F020(u8 *color, u8 red, u8 green, u8 blue);
void func_8014F030(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
void func_8014F180(u16 scale, SVECTOR *positions, SVECTOR *velocities, u16 count);
void func_8014F2D4(SVECTOR *vertices, SVECTOR *offset);
void func_8014F358(SVECTOR *vertices, u16 size);
void func_8014F3E8(SVECTOR *vertices, u16 width, u16 height, u16 depth);
void func_8014F490(SVECTOR *vertices, s32 width, s16 height);
s32 func_8014F524(s16 base, u16 exponent);
void func_8014F564(u16 *output, GsIMAGE *image, s32 mode);
void func_8014F5D0(u16 *output, GsIMAGE *image);
void func_8014F608(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
void func_8014F754(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 z_scale, u16 count);
void func_8014F89C(SVECTOR *vectors, u16 x_scale, u16 y_scale, u16 count);
void func_8014F9A0(SVECTOR *vertices, u16 top_width, u16 bottom_width,
                    u16 top_height, u16 bottom_height, s16 *widths, s16 *heights);
void func_8014FA3C(SVECTOR *vertices, u16 top_width, u16 bottom_width,
                    u16 top_height, s16 *widths, s16 *heights);
void func_8014FABC(u16 radius, u16 spread, u16 height, u16 count, SVECTOR *vertices);
void func_8014FE00(SVECTOR *vertices, u16 size, u16 index);
void func_8014FED4(SVECTOR *first, SVECTOR *second);
void func_801513F4(MATRIX *world, MATRIX *saved, SVECTOR *position,
                    SVECTOR *rotation, VECTOR *scale, u16 mode);
void func_801514BC(MATRIX *source, VECTOR *scale);
void func_801514F8(MATRIX *source, MATRIX *destination);

#endif
