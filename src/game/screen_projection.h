#ifndef MEMORIES_DECOMP_SCREEN_PROJECTION_H
#define MEMORIES_DECOMP_SCREEN_PROJECTION_H

#include "../types.h"
#include "../ygo_types.h"
#include "../psyq/libgte.h"

/* The light-source matrix GsSetLsMatrix is handed. Five callers --
 * display_projection.c, both functions in duel_ground_projection.c, duel_project_card_slot_screen_x.c and
 * duel_field_card_render.c -- each declared it u8[] and cast it to (MATRIX *) at the
 * call, so the SDK signature already fixed the type at every site; the cast
 * was the type escaping the declaration. model_slot_support.c spelled it as
 * a scalar u8 and returned its address as void *. library_draw_wireframe_globe.c declared it
 * with this type but privately, which is the same statement written twice.
 * The last two took this header on 2026-09-11, each measured byte-identical;
 * both build at gcc_2_8_1_g0. */
extern MATRIX gGraphics_LightSourceMatrix;

/* Returns &gGraphics_LightSourceMatrix through the definition's existing void * interface.
 * The background renderer converts that object pointer to the SDK's
 * MATRIX * parameter of GsSetLsMatrix; no alternate allocation is involved. */
void *Model_GetLightSourceMatrix(void);

#endif
