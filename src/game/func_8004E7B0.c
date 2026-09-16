#include "../types.h"
#include "model.h"
#include "model_background.h"
#include "model_graphics_state.h"
#include "func_8004E7B0.h"

/* These historical COMMON definitions retain the post-mfhi delay slot.
 * The graphics state binds to its strong .sdata owner and the snapshot
 * names to existing image/layout symbols; they add no replacement storage.
 * The named profile's --use-comm-section spelling is load-bearing. */
u8 *D_8009AF88;
s16 D_8009AF8E;
s16 D_8009AF90;

ModelBytes8 D_8009B478;
s16 D_8009B47A;
s16 D_8009B47C;
ModelBytes8 D_8009B480;
s16 D_8009B482;
s16 D_8009B484;

/*
 * Direct signed angle differences and the semantic absolute-value predicate
 * recover the initial loads and branch-delay copy without register bindings.
 * Each difference is between signed halfwords, so abs cannot receive INT_MIN.
 * All 496 bytes match with the existing uniform G8 split COMMON profile.
 */
void func_8004E7B0(s32 force)
{
    s32 dy;
    s32 dz;
    s32 t;
    s32 a;
    s32 b;
    s32 u;
    s32 r;
    s32 m;

    dy = D_8009B47A - D_8009B482;
    dz = D_8009B47C - D_8009B484;
    if (__builtin_abs(dy) >= 0x801) {
        if (dy > 0) {
            dy -= MODEL_ANGLE_FULL_TURN;
        } else {
            dy += MODEL_ANGLE_FULL_TURN;
        }
    }
    if (force != 0 || dy != 0) {
        m = ((ModelBackgroundRecord *)D_8009AF88)->texture_width;
        D_8009AF8E = (dy * 1280 / 384 + D_8009AF8E + m) % m;
    }
    if (force != 0 || dz != 0) {
        a = D_8009B47C;
        if (a < 0x801) {
            goto low;
        }
        t = a - MODEL_ANGLE_FULL_TURN;
        if (t < 0) {
            goto neg_side;
        }
        r = -0x139;
        if (t < MODEL_ANGLE_QUARTER_TURN) {
            goto compute;
        }
        goto store;
neg_side:
        r = -0x139;
        if (MODEL_ANGLE_FULL_TURN - a >= MODEL_ANGLE_QUARTER_TURN) {
            goto store;
        }
compute:
        b = D_8009B47C;
        u = b - MODEL_ANGLE_FULL_TURN;
        if (u < 0) {
            goto sub_form;
        }
        r = -(u * 26) / 85;
        goto store;
sub_form:
        r = -((MODEL_ANGLE_FULL_TURN - b) * 26) / 85;
        goto store;
low:
        if (a >= MODEL_ANGLE_QUARTER_TURN) {
            goto clamp_pos;
        }
        r = (a * 26) / 85;
        goto store;
clamp_pos:
        r = 0x139;
store:
        D_8009AF90 = r;
    }
    D_8009B480 = D_8009B478;
}
