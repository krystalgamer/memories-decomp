#ifndef MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H
#define MEMORIES_DECOMP_MODEL_UPDATE_VIEW_METRICS_H

#include "../types.h"
#include "camera_view.h"

struct _GsCOORDUNIT;

/* Measured six-byte prefix only; wider snapshot copies retain their own view. */
typedef struct {
    u16 distance;
    u16 yaw;
    u16 pitch;
} ModelViewMetrics;

typedef char ModelViewMetrics_size_must_be_0x6[
    sizeof(ModelViewMetrics) == 0x6 ? 1 : -1
];
typedef char ModelViewMetrics_alignment_must_match_halfwords[
    __alignof__(ModelViewMetrics) == __alignof__(u16) ? 1 : -1
];
typedef char ModelViewMetrics_distance_offset_must_be_0x0[
    (u32)&((ModelViewMetrics *)0)->distance == 0x0 ? 1 : -1
];
typedef char ModelViewMetrics_yaw_offset_must_be_0x2[
    (u32)&((ModelViewMetrics *)0)->yaw == 0x2 ? 1 : -1
];
typedef char ModelViewMetrics_pitch_offset_must_be_0x4[
    (u32)&((ModelViewMetrics *)0)->pitch == 0x4 ? 1 : -1
];

/* Recomputes the cached distance, yaw and pitch between a view's viewpoint
 * and its reference point. Passing 0 recomputes them for the current view,
 * gModel_RView; passing another view installs it first.
 *
 * The argument's SDK type is supplied by camera_view.h. The body copies
 * that whole record and reads its named members through scalar views. See
 * notes/research/matching-evidence.md for why those reads take the member's
 * address.
 */
/* The three cached values that function recomputes, in address order.
 *
 * The arithmetic in model_update_view_metrics.c fixes what each one is: with
 * `m` the GsRVIEW2, gModel_wViewDistance takes SquareRoot0 of the squared differences
 * between viewpoint and reference point, gModel_wViewYaw takes ratan2 of the
 * difference at +8 (z), and gModel_wViewPitch takes ratan2 of the difference at +4
 * (y) -- three values, computed in that order, for the distance, yaw and
 * pitch this header already names above.  Both angles are then reduced
 * modulo MODEL_ANGLE_FULL_TURN.
 *
 * All three are halfwords.  Every read casts through `*(s16 *)&` or
 * `*(u16 *)&` even in the files that declared the base as `u8`, and
 * model_scene_controllers.c assigns them directly as `unsigned short`, so the
 * byte-wide declarations were a base for the casts rather than a claim about
 * the object.
 */
extern u16 gModel_wViewDistance;
extern u16 gModel_wViewYaw;
extern u16 gModel_wViewPitch;

/* Returns the known distance/yaw/pitch prefix at &gModel_wViewDistance. Separate
   interior labels and eight-byte snapshot views remain unchanged. */
ModelViewMetrics *Model_GetViewMetricsBuffer(void);

void Model_UpdateViewMetrics(GsRVIEW2 *view);

/* Builds a GsCOORDUNIT from an input vector: arg2 is the SVECTOR read in,
   arg3 is the unit written out. The input follows RotTransSV's SDK type;
   the output retains its compatible tagged coordinate-unit declaration.
   model_get_slot0_offset_world_position.c is the only outside consumer
   and is the sole observed call, passing index 0 and arg1 15 with both
   pointers into one stack scratch block.
   Those two leading scalars keep their address-based names because a single
   call site fixes their values without showing what either selects. */
void func_800580D4(s32 index, s32 arg1, SVECTOR *arg2, struct _GsCOORDUNIT *arg3);

#endif
