#ifndef MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H
#define MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H

#include "../types.h"

struct _GsCOORDUNIT;

/* Recomputes the cached distance, yaw and pitch between a view's viewpoint
 * and its reference point. Passing 0 recomputes them for the current view,
 * D_800F56F0; passing another view installs it first.
 *
 * The argument is a GsRVIEW2, which the body proves by assigning through
 * that type. It is declared as a byte pointer only because GsRVIEW2 is an
 * anonymous typedef that this header cannot forward declare; the body names
 * every member through a local GsRVIEW2 *. See
 * notes/research/matching-evidence.md for why those reads take the member's
 * address.
 */
/* The three cached values that function recomputes, in address order.
 *
 * The arithmetic in model_update_view_metrics.c fixes what each one is: with
 * `m` the GsRVIEW2, D_8009B478 takes SquareRoot0 of the squared differences
 * between viewpoint and reference point, D_8009B47A takes ratan2 of the
 * difference at +8 (z), and D_8009B47C takes ratan2 of the difference at +4
 * (y) -- three values, computed in that order, for the distance, yaw and
 * pitch this header already names above.  Both angles are then reduced
 * modulo MODEL_ANGLE_FULL_TURN.
 *
 * All three are halfwords.  Every read casts through `*(s16 *)&` or
 * `*(u16 *)&` even in the files that declared the base as `u8`, and
 * func_80052D2C.c assigns them directly as `unsigned short`, so the
 * byte-wide declarations were a base for the casts rather than a claim about
 * the object.
 */
extern u16 D_8009B478;
extern u16 D_8009B47A;
extern u16 D_8009B47C;

/* Returns &D_8009B478, the first of the three halfwords above. The return
   type is void * because that is what the definition returns, which is how
   three callers each spelled the result differently -- unsigned short * and
   u8 * -- without anything complaining. */
void *func_80059208(void);

void Model_UpdateViewMetrics(u8 *view);

/* Builds a GsCOORDUNIT from a rotation vector: arg2 is the SVECTOR read in,
   arg3 is the unit written out. arg3 is declared through the struct tag, as
   model.h does, because GsCOORDUNIT itself needs libhmd.h; SVECTOR has no
   tag to forward declare, so arg2 stays a byte pointer. func_80059B90.c is the only outside consumer
   and is the sole observed call, passing index 0 and arg1 15 with both
   pointers into one stack scratch block.
   Those two leading scalars keep their address-based names because a single
   call site fixes their values without showing what either selects. */
void func_800580D4(s32 index, s32 arg1, u8 *arg2, struct _GsCOORDUNIT *arg3);

#endif
