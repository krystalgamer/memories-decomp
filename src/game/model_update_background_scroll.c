#include "../types.h"
#include "model.h"
#include "model_background.h"
#include "model_graphics_state.h"
#include "model_update_background_scroll.h"

/* These historical COMMON definitions retain the post-mfhi delay slot.
 * The graphics state binds to its strong .sdata owner and the snapshot
 * names to existing image/layout symbols; they add no replacement storage.
 * The named profile's --use-comm-section spelling is load-bearing. */
ModelBackgroundRecord *G32 gModel_pBackgroundRecord;
s16 gModel_wBackgroundScrollX;
s16 gModel_wBackgroundOffsetY;

ModelBytes8 gModel_wViewDistance;
s16 gModel_wViewYaw;
s16 gModel_wViewPitch;
ModelBytes8 gModel_ViewSnapshot;
s16 gModel_wViewYawSnapshot;
s16 gModel_wViewPitchSnapshot;

/*
 * Direct signed angle differences and the semantic absolute-value predicate
 * recover the initial loads and branch-delay copy without register bindings.
 * Each difference is between signed halfwords, so abs cannot receive INT_MIN.
 * All 496 bytes match with the existing uniform G8 split COMMON profile.
 */
void Model_UpdateBackgroundScroll(s32 force)
{
    s32 dy;
    s32 dz;
    s32 t;
    s32 a;
    s32 b;
    s32 u;
    s32 r;
    s32 m;

    dy = gModel_wViewYaw - gModel_wViewYawSnapshot;
    dz = gModel_wViewPitch - gModel_wViewPitchSnapshot;
    if (__builtin_abs(dy) >= 0x801) {
        if (dy > 0) {
            dy -= MODEL_ANGLE_FULL_TURN;
        } else {
            dy += MODEL_ANGLE_FULL_TURN;
        }
    }
    if (force != 0 || dy != 0) {
        m = gModel_pBackgroundRecord->texture_width;
        gModel_wBackgroundScrollX = (dy * 1280 / 384 + gModel_wBackgroundScrollX + m) % m;
    }
    if (force != 0 || dz != 0) {
        a = gModel_wViewPitch;
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
        b = gModel_wViewPitch;
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
        gModel_wBackgroundOffsetY = r;
    }
    gModel_ViewSnapshot = gModel_wViewDistance;
}
