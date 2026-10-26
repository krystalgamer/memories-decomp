#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/memory.h"
#include "../unmatched.h"
#include "model_effect_coefficients.h"
#include "model_effect_state.h"
#include "camera_view.h"
#include "model_update_view_metrics.h"
#include "model_copy_slot_position.h"
#include "model.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "model_step_view_orbit.h"
#include "model_effect_endpoints.h"
#include "model_effect_requests.h"
#include "model_slot_support.h"
#include "model_keyframe_spline.h"
#include "model_transfer_flags.h"
#include "model_transfer_state.h"
#include "model_queue_relative_camera_orbit.h"
#include "model_cubic_coefficients.h"
#include "model_get_slot_readiness.h"

#define MODEL_EFFECT_ENDPOINT_VIEW(value) ((ModelEffectEndpoint *)(value))
#define KEY_BYTES(key) ((u8 *)(key))

/*
 * Semantic absolute value and keyframe bounds: `Model_PrepareEffectKey`
 *
 * The 1004-byte keyframe routine matches all 251 instruction words under the
 * existing uniform `gcc_2_8_1_g8_split` profile. Its parameter uses the
 * canonical 0x28-byte `Key` layout shared with the evaluator. The duration
 * field at +0x22 is consumed as a denominator by the evaluator below; the
 * retired candidate's "audible radius" description was not the established
 * contract.
 *
 * Two `__builtin_abs` calls are load-bearing. They use the same semantic
 * intrinsic as matching `duel_card_and_status_draw.c` and `sd_apply_positional_sound.c`.
 * The compiler emits its `abs` pseudo-instruction, rather than a hand-written
 * conditional whose scheduled delay slot duplicates a multiply. Both operands
 * come from signed halfwords promoted to s32, including -32768, so their
 * magnitudes fit the result type.
 *
 * Case-local distance temporaries and per-case bound checks preserve the
 * multiply/result registers and let the bound loads fill their original
 * slots. Pose traversal is indexed from `gModel_aPoseSnapshot`, and the path counter is
 * initialized to one before its byte offset is reset. With those source
 * dependencies recovered, ordinary G8 remains five address words away, while
 * the existing split profile materializes the common pose base exactly.
 *
 * The global active-key guard remains separate from the input key, and the
 * initial threshold still tests the input magnitude against 0x4000, not a
 * replacement `min(scale, 0x4000)` expression. All six canonical attempts are
 * preserved; one post-terminal resolution records this result. No register
 * pins, source-level inline assembly, symbol aliases, or new profile is used.
 */
void Model_PrepareEffectKey(Key *state)
{
    /* The copier writes four halfwords; only the first three are coordinates. */
    s16 pos[4];
    s16 buf[10][3];
    Coeff *e;
    SVECTOR *g;
    s32 i;
    s32 j;
    s32 a;
    s32 k;
    s32 sum;
    u32 d;
    s32 dx;
    s32 dy;
    s32 dz;
    s32 slot;
    u16 v;

    {
        s32 n;
        s32 t;
        n = state->magnitude;
        a = __builtin_abs(n);
        t = a;
        if (a <= 0) {
            t = 1;
        }
        k = t * 2;
    }
    Model_RefreshEffectKeySlotValues(state);
    state->progress = 0;
    if (gModel_pCurrentEffectKey->ready != 0) {
        return;
    }
    state->ready = 1;
    if (a >= 0x4000) {
        state->duration = 0x4000;
    } else {
        state->duration = k;
    }
    if (state->magnitude >= 0) {
        return;
    }

    /* Coeff preserves the aligned halfword loads of these packed endpoints. */
    e = (Coeff *)state;
    for (i = 0; i < 2; i++, e++) {
        g = &gModel_aPoseSnapshot[i];
        switch ((s16)e->w) {
        case 0x80:
        case 0x81:
        {
            s32 dx, dy, dz;
            slot = (s16)(e->w & 0xFF7F);
            if (Model_GetSlotReadiness(slot) != 1) {
                continue;
            }
            Model_CopySlotPosition(slot, (u16 *)pos);
            pos[0] = pos[0] + (u16)e->x;
            pos[1] = pos[1] + (u16)e->y;
            pos[2] = pos[2] + (u16)e->z;
            dx = pos[0] - g->vx;
            dy = pos[1] - g->vy;
            dz = pos[2] - g->vz;
            d = k * SquareRoot0(dx * dx + dy * dy + dz * dz) / 1000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        }
        case 1:
            sum = 0;
            for (j = 0; j < 10; j++) {
                Model_EvaluateKeyframeSpline(state, i, j, 0x1E, buf[j]);
            }
            for (j = 1; j < 10; j++) {
                dx = buf[j - 1][0] - buf[j][0];
                dy = buf[j - 1][1] - buf[j][1];
                dz = buf[j - 1][2] - buf[j][2];
                sum += SquareRoot0(dx * dx + dy * dy + dz * dz);
            }
            d = k * sum / 1000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        case 4:
        {
            s32 radius_scaled;
            s32 factor;
            v = (u16)gModel_aPoseSnapshot[2].vx;
            if (gModel_aPoseSnapshot[2].vx < e->x) {
                v = (u16)e->x;
            }
            radius_scaled = (s16)v * 6284 / 1000;
            d = k * radius_scaled;
            factor = __builtin_abs(e->y);
            d = (u32)(d * factor) / 4096000;
            if (state->duration < d) {
                state->duration = d;
            }
            continue;
        }
        default:
            continue;
        }
    }
}

/* Evaluates one channel of the keyframe ring that Model_ApplyEffectKeyQueueCommand fills and
   Model_CaptureKeyframePose seeds. `cur` points at a 0x28-byte key inside gModel_aEffectKeys, so
   `cur - gModel_aEffectKeys` is the ring index; the three keys the segment spans are
   that index and the next two modulo the live key count in gModel_bEffectKeyCount. Keys
   whose 0x8-byte channel record has kind 1 at +6 terminate the segment, and
   the tail is padded with the last real key. The four control points handed to
   Model_BuildCubicCoefficients are the current pose at gModel_aPoseSnapshot followed by those three
   keys, except that a non-zero `den` also replaces the first with the previous
   key when that one is kind 1. With `den` zero the divisor is instead the sum
   of the distinct keys' +0x22 durations. Each of the three components is then
   Horner-evaluated at scale/den and added to the constant term. */
void Model_EvaluateKeyframeSpline(Key *cur, s32 k, s32 scale, s32 den, s16 *out)
{
    Key *keys[3] = {
        &gModel_aEffectKeys[(cur - gModel_aEffectKeys) % gModel_bEffectKeyCount],
        &gModel_aEffectKeys[(cur - gModel_aEffectKeys + 1) % gModel_bEffectKeyCount],
        &gModel_aEffectKeys[(cur - gModel_aEffectKeys + 2) % gModel_bEffectKeyCount]
    };
    s16 *pts[4];
    Coeff co[3];
    s32 i;
    s32 j;

    memset(pts, 0, 16);
    pts[0] = (s16 *)(
        (u8 *)gModel_aPoseSnapshot + (u32)&((u8 (*)[8])0)[k]);
    for (i = 1; i < 3; i++) {
        s16 *e;

        e = (s16 *)(
            KEY_BYTES(keys[i]) + (u32)&((u8 (*)[8])0)[k]);
        if (e[3] != 1) {
            break;
        }
    }
    j = i - 1;
    if (i < 3) {
        for (; i < 3; i++) {
            keys[i] = keys[j];
        }
    }
    for (i = 0; i < 3; i++) {
        pts[i + 1] = (s16 *)(
            KEY_BYTES(keys[i]) + (u32)&((u8 (*)[8])0)[k]);
    }
    if (den != 0) {
        Key *kp;
        s16 *p;

        kp = &gModel_aEffectKeys[(cur - gModel_aEffectKeys + gModel_bEffectKeyCount - 1) % gModel_bEffectKeyCount];
        p = (s16 *)(KEY_BYTES(kp) + (u32)&((u8 (*)[8])0)[k]);
        if (p[3] == 1) {
            pts[0] = p;
        }
        i = 0;
    } else {
        den = keys[0]->duration;
        for (i = 1; i < 3; i++) {
            if (keys[i - 1] != keys[i]) {
                den += keys[i]->duration;
            }
        }
        i = 0;
    }
    for (i = 0; i < 3; i++) {
        s32 v;

        Model_BuildCubicCoefficients(pts[0][i], pts[1][i], pts[2][i], pts[3][i], &co[i], 2);
        v = co[i].x * scale / den;
        v = (v + co[i].y) * scale / den;
        v = (v + co[i].z) * scale / den;
        out[i] = co[i].w + v;
    }
}

/* Refreshes the current pose at gModel_aPoseSnapshot that the evaluator above uses as
   its first control point, from the two source records returned by
   Model_GetCameraViewBuffer and Model_GetViewMetricsBuffer, and optionally
   refreshes up to two resolved endpoints in gModel_pCurrentEffectKey from model slots.
   This is the seed half of the ring the evaluator reads, which is why the two share a
   translation unit. */
void Model_CaptureKeyframePose(s32 capture_endpoints)
{
    GsRVIEW2 *left = Model_GetCameraViewBuffer();
    ModelViewMetrics *right = Model_GetViewMetricsBuffer();

    gModel_aPoseSnapshot[0].vx = left->vpx;
    gModel_aPoseSnapshot[0].vy = left->vpy;
    gModel_aPoseSnapshot[0].vz = left->vpz;
    gModel_aPoseSnapshot[1].vx = left->vrx;
    gModel_aPoseSnapshot[1].vy = left->vry;
    gModel_aPoseSnapshot[1].vz = left->vrz;
    gModel_aPoseSnapshot[2].vx = right->distance;
    gModel_aPoseSnapshot[2].vy = right->yaw;
    gModel_aPoseSnapshot[2].vz = right->pitch;
    if (capture_endpoints != 0) {
        Key *key = gModel_pCurrentEffectKey;
        if (key != (Key *)0) {
            s32 i = 0;
            s32 offset = 16;
            /* A byte cursor over key->requested: ModelEffectEndpoint is
               packed, so reading its members costs byte loads (+0x30). */
            u8 *entry = KEY_BYTES(key);
            for (; i < 2; offset += 8, i++, entry += 8) {
                s32 kind = *(s16 *)(entry + 6);
                if (kind < 4) {
                    if (kind >= 2)
                        Model_CopySlotPosition(
                            *(s16 *)entry, (u16 *)(KEY_BYTES(key) + offset)
                        );
                }
            }
        }
    }
}

s32 Model_GetCameraHoldState(void)
{
    return gModel_bCameraHoldState;
}

void Model_SetCameraHoldState(s32 value)
{
    gModel_bCameraHoldState = value;
}

s32 Model_GetHandlerCallbackState(void)
{
    return gModel_bHandlerCallbackState;
}

void Model_SetHandlerCallbackState(s32 value)
{
    gModel_bHandlerCallbackState = value;
}

ModelCameraOrbitPreset *ModelEffect_GetCoefficient(s32 index)
{
    return &gModel_aCameraOrbitPresets[index];
}

s32 Model_AdjustCameraDistanceForSlot(s32 level, s32 value)
{
    ModelEffectAdjustment local;
    s16 delta;

    if (level >= 2) {
        return value;
    }

    Model_GetSlotDimensionsWithOverrides(level, (s16 *)&local);

    if (local.max < 50) {
        local.max = 50;
    }

    local.max -= 300;
    delta = local.max;

    if (delta != 0) {
        s32 divisor = 750;
        s32 half = value;

        if (delta > 0) {
            half = (s32)(value + ((u32)value >> 31)) >> 1;
        }
        value += (delta * half) / divisor;
    }

    return value;
}

void Model_SetCameraOrbitFromPreset(s32 arg0, s32 arg1, SVECTOR *arg2)
{
    ModelCameraOrbitPreset *r;
    s32 v;
    s32 d;
    s32 t;
    s32 k;
    s32 u;
    ModelEffectAdjustment sp18;

    r = &gModel_aCameraOrbitPresets[arg1];

    if (gModel_bCameraHoldState == 1) {
        if (gModel_bHandlerCallbackState == 1) {
            return;
        }
    }

    v = r->distance;

    if (arg0 < 2) {
        Model_GetSlotDimensionsWithOverrides(arg0, (s16 *)&sp18);
        if (sp18.max < 0x32) {
            sp18.max = 0x32;
        }
        sp18.max -= 0x12C;
        d = sp18.max;
        if (d != 0) {
            k = 750;
            t = v;
            if (d > 0) {
                t = v / 2;
            }
            u = d * t;
            t = u;
            v += t / k;
        }
    }

    Model_SetCameraOrbit(arg0, v, r->yaw, r->pitch, arg2);
}

void Model_SetCameraOrbit(int mode, int y, int a, int b, SVECTOR *offset)
{
    GsRVIEW2 *p = Model_GetCameraViewBuffer();
    ModelViewMetrics *q = Model_GetViewMetricsBuffer();
    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1)
        return;
    if (mode >= 0) {
        short v[4];
        Model_CopySlotPosition(mode, (u16 *)v);
        p->vrx = v[0];
        p->vry = v[1];
        p->vrz = v[2];
    }
    if (offset) {
        int sign = -1;
        if (mode <= 0)
            sign = 1;
        p->vrx += offset->vx * sign;
        p->vry += offset->vy;
        p->vrz += offset->vz * sign;
    }
    q->distance = y;
    q->yaw = (a + MODEL_ANGLE_FULL_TURN +
            (mode <= 0 ? 3 * MODEL_ANGLE_QUARTER_TURN :
                         MODEL_ANGLE_QUARTER_TURN)) %
        MODEL_ANGLE_FULL_TURN;
    q->pitch = (b + MODEL_ANGLE_FULL_TURN) % MODEL_ANGLE_FULL_TURN;
    Model_StepViewOrbit(1, MODEL_ANGLE_FULL_TURN, 0, 0, 1);
    Model_CaptureKeyframePose(1);
    Model_RequestCameraMove(-1);
}

int Model_NextEffectKeyQueueMode(void)
{
    if (gModel_nEffectKeyBatchOrdinal < 0) {
        return 0;
    }
    return gModel_nEffectKeyBatchOrdinal++ > 0;
}

void Model_SetEffectKeyBatchingEnabled(int value)
{
    if (gModel_bCameraHoldState != 1 || gModel_bHandlerCallbackState != 1) {
        if (value == 0) {
            gModel_nEffectKeyBatchOrdinal = -1;
        } else {
            gModel_nEffectKeyBatchOrdinal = 0;
        }
    }
}

/* Applies the same slot-dependent camera-distance adjustment, then queues
   relative orbit endpoints through Model_QueueRelativeCameraOrbit. */
void Model_QueueCameraOrbitFromPreset(s32 arg0, s32 arg1, SVECTOR *offset, s32 arg3) {
    ModelCameraOrbitPreset *rec = &gModel_aCameraOrbitPresets[arg1];
    s32 s0;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == gModel_bCameraHoldState) {
        return;
    }

    s0 = rec->distance;

    if (arg0 < 2) {
        ModelEffectAdjustment adjustment;
        s32 v1;

        Model_GetSlotDimensionsWithOverrides(arg0, (s16 *)&adjustment);
        if (adjustment.max < 0x32) {
            adjustment.max = 0x32;
        }
        adjustment.max -= 300;
        v1 = adjustment.max;
        if (v1 != 0) {
            s32 divisor = 750;
            s32 half = (v1 > 0) ? (s0 / 2) : s0;
            s0 += (v1 * half) / divisor;
        }
    }

    Model_QueueRelativeCameraOrbit(arg0, s0, rec->yaw, rec->pitch, offset, arg3);
}

void Model_QueueEffectEndpointPair(s32 a, s32 b, s32 c)
{
    ModelEffectEndpoint *x =
        a < 0 ? 0 : &gModel_aEffectEndpoints[a];
    ModelEffectEndpoint *y =
        b < 0 ? 0 : &gModel_aEffectEndpoints[b];
    s32 flag;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1) {
        return;
    }
    flag = (s8)gModel_nEffectKeyBatchOrdinal;
    if (flag < 0) {
        flag = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal++;
        flag = flag > 0;
    }
    Model_ApplyEffectKeyQueueCommand(flag, x, y, c);
}

void Model_QueueCameraShake(s32 value, s32 arg)
{
    /* The aligned array keeps retail's four halfword stores; a packed local
       would expand them into byte stores. */
    s16 d[4] = {value, 0, 0, 5};
    s32 flag;
    s32 next;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == 1) {
        return;
    }
    flag = (s8)gModel_nEffectKeyBatchOrdinal;
    next = (u8)gModel_nEffectKeyBatchOrdinal;
    if (flag < 0) {
        flag = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = next + 1;
        flag = flag > 0;
    }
    Model_ApplyEffectKeyQueueCommand(
        flag, MODEL_EFFECT_ENDPOINT_VIEW(&d), MODEL_EFFECT_ENDPOINT_VIEW(&d), arg
    );
}

void Model_QueueEffectEndpointSequences(
    s32 count, ModelEffectEndpoint *p1, ModelEffectEndpoint *p2, s32 arg3
)
{
    s8 raw172;
    s32 flag;

    if (gModel_bCameraHoldState == 1 && gModel_bHandlerCallbackState == gModel_bCameraHoldState) {
        return;
    }

    raw172 = gModel_nEffectKeyBatchOrdinal;
    if (raw172 < 0) {
        flag = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = (u8)gModel_nEffectKeyBatchOrdinal + 1;
        flag = raw172 > 0;
    }

    if (count == 0) {
        return;
    }

    do {
        Model_ApplyEffectKeyQueueCommand(
            flag,
            p1,
            p2,
            arg3
        );

        if (p1 != 0) {
            p1 = p1 + 1;
        }
        if (p2 != 0) {
            p2 = p2 + 1;
        }

        if (p1 != 0 &&
            *(s16 *)&p1->kind != 0) {
            goto cont;
        }
        if (p2 == 0) {
            return;
        }
        if (*(s16 *)&p2->kind == 0) {
            return;
        }
cont:
        if (count > 0) {
            count--;
        }
        flag = 1;
    } while (count != 0);
}

void Model_ApplyEffectKeyQueueCommand(
    s32 arg0,
    ModelEffectEndpoint *arg1,
    ModelEffectEndpoint *arg2,
    s32 arg3
)
{
    u8 *p;
    u8 *q;
    u8 *r;
    s32 n;
    s32 i;
    s32 one;

    one = 1;
    if (gModel_bCameraHoldState == one && gModel_bHandlerCallbackState == one) {
        return;
    }

    if (arg0 == one) {
        goto m1;
    }
    if (arg0 < 2) {
        if (arg0 == 0) {
            goto m0;
        }
        return;
    }
    if (arg0 == 2) {
        goto m2;
    }
    return;

m0:
    Model_RequestCameraMove(-1);
    gModel_pCurrentEffectKey = gModel_aEffectKeys;
    gModel_bEffectKeyCount = 0;
    if (arg1 == (ModelEffectEndpoint *)0 &&
        arg2 == (ModelEffectEndpoint *)0 &&
        (arg3 == 0 || arg3 == 0x4000)) {
        return;
    }
    Model_CaptureKeyframePose(0);
    gModel_bEffectKeyPreparationPending = 1;

m1:
    if (gModel_bEffectKeyCount < 0xA) {
        Key *key = &gModel_aEffectKeys[gModel_bEffectKeyCount];
        r = KEY_BYTES(key);
        if (arg1 != (ModelEffectEndpoint *)0) {
            key->requested[0] = *arg1;
        } else {
            key->requested[0].kind = 0;
        }
        if (arg2 != (ModelEffectEndpoint *)0) {
            key->requested[1] = *arg2;
            key->ready = 0;
        } else {
            key->requested[1].kind = 0;
            key->ready = 0;
        }
        n = gModel_bEffectKeyCount;
        key->magnitude = arg3;
        key->duration = 0;
        key->progress = 0;
        gModel_bEffectKeyCount = n + 1;
    }
    return;

m2:
    q = KEY_BYTES(gModel_aEffectKeys);
    for (i = 0; i < gModel_bEffectKeyCount; i++) {
        p = q;
        q += 0x28;
        Model_PrepareEffectKey((Key *)p);
    }
    gModel_bEffectKeyPreparationPending = 0;
    gModel_nEffectKeyBatchOrdinal = -1;
}

void Model_ResetEffectKeyQueue(void)
{
    gModel_pCurrentEffectKey = 0;
    gModel_bEffectKeyCount = 0;
    gModel_bEffectKeyPreparationPending = 0;
    gModel_nEffectKeyBatchOrdinal = -1;
    gModel_bCameraHoldState = 0;
    gModel_bHandlerCallbackState = 0;
}

int Model_IsEffectKeyQueueEmpty(void)
{
    return gModel_bEffectKeyCount == 0;
}

int Model_GetCurrentEffectKeyAddress(void)
{
    int value = 0;

    if (gModel_bEffectKeyCount != 0) {
        value = (s32)gModel_pCurrentEffectKey;
    }
    return value;
}

void Model_RefreshEffectKeySlotValues(Key *key)
{
    u8 *data = KEY_BYTES(key);
    int i;

    if (!data) {
        data = KEY_BYTES(gModel_pCurrentEffectKey);
    }
    if (!data) {
        return;
    }
    for (i = 0; i < 2; i++) {
        ModelEffectEndpoint *item =
            (ModelEffectEndpoint *)(data + i * sizeof(ModelEffectEndpoint));

        /* Key storage is halfword-aligned; packed member reads use bytes. */
        if (*(s16 *)&item->kind < 4) {
            if (*(s16 *)&item->kind >= 2) {
                Model_CopySlotPosition(
                    *(s16 *)&item->x,
                    (u16 *)(data + 0x10 + i * sizeof(u16[4]))
                );
            }
        }
    }
}

void Model_BuildCubicCoefficients(int a, int b, int c, int d, Coeff *out, int scale)
{
    int x = (b - c) * scale;
    int y = (a + c - b * 2) * scale;
    int z = (b - a) * scale;
    out->w = a;
    out->x = x - a + d;
    out->y = y;
    out->z = z;
}
