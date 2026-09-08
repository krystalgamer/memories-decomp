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
void Model_UpdateViewMetrics(u8 *view);

#endif
