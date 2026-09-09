#ifndef MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H
#define MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H

#include "../types.h"

/* Recomputes the cached distance, yaw and pitch between a view's viewpoint
 * and its reference point. Passing 0 recomputes them for the current view,
 * D_800F56F0; passing another view installs it first.
 *
 * The argument is a GsRVIEW2, which the body proves by assigning through
 * that type, but it is declared as a byte pointer because the body reaches
 * the fields by offset and converting it to member access moves a register.
 * See notes/research/matching-evidence.md.
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
 * model_scene_setup.c assigns them directly as `unsigned short`, so the
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

#endif
