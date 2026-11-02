#include "../types.h"
#include "model_update_view_metrics.h"
#include "model_copy_slot_position.h"
#include "../psyq/memory.h"
#include "model.h"
#include "model_transfer_flags.h"
#include "model_queue_relative_camera_orbit.h"
#include "model_effect_state.h"

void Model_QueueRelativeCameraOrbit(
    s32 slot_selector,
    s32 distance,
    s32 yaw,
    s32 pitch,
    SVECTOR *target_offset,
    s32 key_magnitude
) {
    u16 target_halfwords[4];
    u16 orbit_halfwords[4];
    ModelViewMetrics *view_metrics;
    s32 one;
    s32 offset_sign;
    s32 biased_yaw;
    s32 biased_pitch;
    s32 yaw_word;
    s32 x;
    s32 queue_mode;

    view_metrics = Model_GetViewMetricsBuffer();
    memset(target_halfwords, 0, sizeof(target_halfwords));
    target_halfwords[3] = slot_selector | 0x80;
    one = 1;

    if (gModel_bCameraHoldState == one && gModel_bHandlerCallbackState == one) {
        return;
    }

    if (slot_selector == 2) {
        Model_CopySlotPosition(2, (u16 *)target_halfwords);
        target_halfwords[3] = one;
    }

    if (target_offset != 0) {
        offset_sign = -1;
        if (slot_selector <= 0) {
            offset_sign = 1;
        }
        target_halfwords[0] = target_halfwords[0] + target_offset->vx * offset_sign;
        /* Retail zero-extends only the middle component. */
        target_halfwords[1] = target_halfwords[1] + *(u16 *)&target_offset->vy;
        target_halfwords[2] = target_halfwords[2] + target_offset->vz * offset_sign;
    }

    orbit_halfwords[0] = distance;
    x = (s16)view_metrics->yaw;
    biased_yaw = yaw + MODEL_ANGLE_FULL_TURN;
    if (slot_selector <= 0) {
        x = x - 0xC00;
    } else {
        x = x - 0x400;
    }
    orbit_halfwords[1] = (biased_yaw - x) - (biased_yaw - x) / MODEL_ANGLE_FULL_TURN *
        MODEL_ANGLE_FULL_TURN;
    yaw_word = orbit_halfwords[1];
    if (*(s16 *)&orbit_halfwords[1] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        orbit_halfwords[1] = yaw_word - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&orbit_halfwords[1] < -MODEL_ANGLE_HALF_TURN) {
        orbit_halfwords[1] = orbit_halfwords[1] + MODEL_ANGLE_FULL_TURN;
    }

    biased_pitch = pitch + MODEL_ANGLE_FULL_TURN;
    orbit_halfwords[2] = (biased_pitch - biased_pitch / MODEL_ANGLE_FULL_TURN * MODEL_ANGLE_FULL_TURN) -
        view_metrics->pitch;
    if (*(s16 *)&orbit_halfwords[2] >= MODEL_ANGLE_WRAP_THRESHOLD) {
        orbit_halfwords[2] = orbit_halfwords[2] - MODEL_ANGLE_FULL_TURN;
    }
    if (*(s16 *)&orbit_halfwords[2] < -MODEL_ANGLE_HALF_TURN) {
        orbit_halfwords[2] = orbit_halfwords[2] + MODEL_ANGLE_FULL_TURN;
    }

    queue_mode = *(s8 *)&gModel_nEffectKeyBatchOrdinal;
    x = queue_mode;
    orbit_halfwords[3] = 4;
    if (x < 0) {
        queue_mode = 0;
    } else {
        gModel_nEffectKeyBatchOrdinal = gModel_nEffectKeyBatchOrdinal + 1;
        queue_mode = x > 0;
    }
    Model_ApplyEffectKeyQueueCommand(
        queue_mode, (ModelEffectEndpoint *)orbit_halfwords, (ModelEffectEndpoint *)target_halfwords, key_magnitude
    );
}

/* The model transfer family's .sdata block, 0x8009B074 to 0x8009B07F.
 *
 * model_transfer_flags.h already settled everything this file needs. It
 * records what each byte is, and it worked out that gModel_pCurrentEffectKey is a pointer
 * rather than the s32 two units had spelled it: every retail read uses the
 * word as a base for a load at a fixed offset or a null test. Queue setup
 * stores gModel_aEffectKeys, while the updater advances and wraps that Key cursor
 * or clears it with the count. This file gives the block a definition and
 * includes its owner header so the two cannot drift apart.
 *
 * The header also answers in advance the question a carve would otherwise
 * have to ask, noting that every retail access to the run is gp-relative.
 * That is what .sdata provides, so no consumer needs a declaration arm.
 *
 * This is the last of the three owners in the 40-byte range at 0x8009B058
 * that the notes call the scattered tail. The middle block belongs to the
 * movie pipeline and the first eight bytes to main_menu_load_package_stage.c,
 * which stays extracted.
 *
 * gModel_nEffectKeyBatchOrdinal is initialised to -1. The header describes it as a signed
 * counter armed at -1 and the retail byte is 0xFF, so the semantics recorded
 * there and the image agree; that agreement is also what makes s8 rather
 * than u8 the right spelling for the definition.
 *
 * The block ends with three bytes no symbol names, so splat's label for
 * gModel_bHandlerCallbackState spans four bytes although the object is one -- the same way the
 * label at D_8009AF18 ran long. They are written out as padding because an
 * object stopping at gModel_bHandlerCallbackState would leave the window three bytes short.
 *
 * The padding is three separate bytes rather than a u8[3], which is not a
 * style choice. An array of three bytes is given four-byte alignment, so it
 * lands at offset 0x0C instead of 0x09 and the compiler fills 0x09 to 0x0B
 * itself; the section then measures fifteen bytes and the link fails with
 * .initialized_data overlapping the bss image. Separate bytes align to one
 * and pack where they belong. model_graphics_state.c spells its own
 * continuation bytes this way for the same reason.
 *
 * Every value is zero apart from that -1, so the section attributes are load
 * bearing rather than decorative: without them these objects would be placed
 * in .sbss and leave .sdata entirely. */

Key *G32 gModel_pCurrentEffectKey = 0;
u8 gModel_bEffectKeyCount = 0;
u8 gModel_bEffectKeyPreparationPending = 0;
s8 gModel_nEffectKeyBatchOrdinal = -1;
u8 gModel_bCameraHoldState = 0;
u8 gModel_bHandlerCallbackState = 0;
static u8 sModelTransferFlags_Pad7D = 0;
static u8 sModelTransferFlags_Pad7E = 0;
static u8 sModelTransferFlags_Pad7F = 0;
