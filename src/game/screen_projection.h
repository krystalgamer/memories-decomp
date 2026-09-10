#ifndef MEMORIES_DECOMP_SCREEN_PROJECTION_H
#define MEMORIES_DECOMP_SCREEN_PROJECTION_H

#include "../types.h"
#include "../psyq/libgte.h"

/* SXY2 as the GTE writes it back through `swc2 $14`. Every projection site in
 * the resident image reads a projected point this way: screen x in the low
 * half of the word, screen y in the high half.
 *
 * The two halves are deliberately not the same type, and that is the whole
 * reason this record exists rather than a plain s32. x is only ever biased and
 * stored, so it stays u16 and its read comes out `lhu`; y is taken through a
 * signed s32 before it is biased, so it is s16 and its read comes out `lh`.
 * src/candidates/func_800178BC.c records the same fact in prose next to its
 * own read.
 *
 * This was ProjectedPair in display_projection.c, ProjectedPair again in
 * func_800177C4.c and Projected in func_800178BC.c: three textually identical
 * definitions of one GTE result layout. */
typedef struct {
    u16 x;
    s16 y;
} ProjectedPair;

/* One entry of D_800EA070, the biased screen position func_800177C4 writes for
 * each card slot after projecting D_800908A0 through the GTE. Signed in both
 * halves: the bias is applied before the store and the result goes negative
 * for a slot that lands off the top or left of the screen. */
typedef struct {
    s16 x;
    s16 y;
} ScreenPair;

/* The light-source matrix GsSetLsMatrix is handed. Four callers -- 
 * display_projection.c, func_800177C4.c, func_800178BC.c and func_8001B0CC.c
 * -- each declared it u8[] and cast it to (MATRIX *) at the call, so the SDK
 * signature already fixed the type at every site; the cast was the type
 * escaping the declaration. model_buffer_getters.c spelled it as a scalar u8
 * and returned its address as void *. */
extern MATRIX D_800FE148;

#endif
