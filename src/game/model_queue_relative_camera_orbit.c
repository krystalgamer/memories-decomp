#include "../types.h"
#include "model_update_view_metrics.h"
#include "model_copy_slot_position.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_queue_relative_camera_orbit.h"
#include "model_effect_state.h"

void Model_QueueRelativeCameraOrbit(
    s32 arg0,
    s32 arg1,
    s32 arg2,
    s32 arg3,
    SVECTOR *offset,
    s32 arg5
) {
    u16 a[4];
    u16 b[4];
    ModelViewMetrics *s;
    s32 one;
    s32 m;
    s32 t;
    s32 q;
    s32 r;
    s32 x;
    s32 first;

    s = Model_GetViewMetricsBuffer();
    memset(a, 0, sizeof(a));
    a[3] = arg0 | 0x80;
    one = 1;

    if (gModel_bCameraHoldState == one && gModel_bHandlerCallbackState == one) {
        return;
    }

    if (arg0 == 2) {
        Model_CopySlotPosition(2, (u16 *)a);
        a[3] = one;
    }

    if (offset != 0) {
        m = -1;
        if (arg0 <= 0) {
            m = 1;
        }
        a[0] = a[0] + offset->vx * m;
        /* Retail zero-extends only the middle component. */
        a[1] = a[1] + *(u16 *)&offset->vy;
        a[2] = a[2] + offset->vz * m;
    }

    b[0] = arg1;
    x = (s16)s->yaw;
    t = arg2 + MODEL_ANGLE_FULL_TURN;
    if (arg0 <= 0) {
        x = x - 0xC00;
    } else {
        x = x - 0x400;
    }
    b[1] = (t - x) - (t - x) / MODEL_ANGLE_FULL_TURN *
        MODEL_ANGLE_FULL_TURN;
    r = b[1];
    if (*(s16 *)&b[1] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        b[1] = r - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&b[1] < -MODEL_ANGLE_HALF_TURN) {
        b[1] = b[1] + MODEL_ANGLE_FULL_TURN;
    }

    q = arg3 + MODEL_ANGLE_FULL_TURN;
    b[2] = (q - q / MODEL_ANGLE_FULL_TURN * MODEL_ANGLE_FULL_TURN) -
        s->pitch;
    if (*(s16 *)&b[2] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        b[2] = b[2] - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&b[2] < -MODEL_ANGLE_HALF_TURN) {
        b[2] = b[2] + MODEL_ANGLE_FULL_TURN;
    }

    first = *(s8 *)&gModel_nEffectKeyBatchOrdinal;
    x = first;
    b[3] = 4;
    if (x < 0) {
        first = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = gModel_nEffectKeyBatchOrdinal + 1;
        first = x > 0;
    }
    Model_ApplyEffectKeyQueueCommand(
        first, (ModelEffectEndpoint *)b, (ModelEffectEndpoint *)a, arg5
    );
}
