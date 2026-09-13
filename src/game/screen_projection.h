#ifndef MEMORIES_DECOMP_SCREEN_PROJECTION_H
#define MEMORIES_DECOMP_SCREEN_PROJECTION_H

#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"

/* The light-source matrix GsSetLsMatrix is handed. Five callers --
 * display_projection.c, func_800177C4.c, func_800178BC.c, func_8001B0CC.c and
 * func_80015EF4.c -- each declared it u8[] and cast it to (MATRIX *) at the
 * call, so the SDK signature already fixed the type at every site; the cast
 * was the type escaping the declaration. model_buffer_getters.c spelled it as
 * a scalar u8 and returned its address as void *. func_80029934.c declared it
 * with this type but privately, which is the same statement written twice.
 * The last two took this header on 2026-09-11, each measured byte-identical;
 * both build at gcc_2_8_1_g0. */
extern MATRIX D_800FE148;

#endif
