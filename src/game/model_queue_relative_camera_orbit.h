#ifndef MEMORIES_DECOMP_MODEL_QUEUE_RELATIVE_CAMERA_ORBIT_H
#define MEMORIES_DECOMP_MODEL_QUEUE_RELATIVE_CAMERA_ORBIT_H

#include "../types.h"
#include "../psyq/libgte.h"

/* Queues a slot-relative reference endpoint and distance/yaw/pitch deltas.
 * The optional SVECTOR offset retains side-dependent x/z signs and the
 * unsigned middle-component read. Scene controllers and the preset wrapper
 * pass the same pointer type; no integer-to-pointer forwarding is involved. */
void Model_QueueRelativeCameraOrbit(
    s32 arg0, s32 arg1, s32 arg2, s32 arg3, SVECTOR *offset, s32 arg5
);

/* Keep the frozen Spanish primary-effect source spelling. */
#define func_8005D994 Model_QueueRelativeCameraOrbit

#endif
