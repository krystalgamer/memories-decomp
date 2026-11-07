/* Model spatial queries: the per-slot camera preset yaw, the spatial sound
 * volume and pan, camera move completion and the packet-buffer space check.
 * In every executable these are one contiguous run of text
 * (0x8005A618-0x8005A98C in the US build). */
#include "../types.h"
#include "model_effect_coefficients.h"
#include "model_effect_wrapped_value.h"
#include "trig_constants.h"

s32 Model_GetSlotCameraPresetYaw(s32 slot_index)
{
    s32 slot_offset = slot_index * MODEL_SLOT_SIZE;
    ModelCameraOrbitPreset *preset;
    s32 yaw;
    ModelSlotCF8TailView *slot_metadata;
    s32 biased_yaw;

    slot_metadata = (ModelSlotCF8TailView *)(slot_offset + (s32)&gModel_aSlots[0].metadata);
    /* Retail rebuilds the alias base for the selector-byte load. */
    preset = ModelEffect_GetCoefficient(
        slot_metadata->prefix.bytes.guardian_star_options[
            (((u8 *)&gModel_aSlots[0].metadata) + slot_offset)[MODEL_SLOT_CF8_DFE_OFFSET]
        ] & 0x1F
    );

    yaw = preset->yaw;
    if (slot_index <= 0) {
        biased_yaw = yaw + TRIG_ANGLE_FULL_TURN +
                 3 * TRIG_ANGLE_QUARTER_TURN;
        return biased_yaw % TRIG_ANGLE_FULL_TURN;
    }

    biased_yaw = yaw + TRIG_ANGLE_FULL_TURN + TRIG_ANGLE_QUARTER_TURN;
    return biased_yaw % TRIG_ANGLE_FULL_TURN;
}

#include "camera_view.h"
#include "../psyq/libgte.h"
#include "sd_calc_spatial_volume_pan.h"

/* Distance-to-camera fade and pan for one sound source: the square root of
 * the distance from the listener record at gModel_RView, minus 700, sets the
 * volume byte (0xFF down to 0x10), and the signed projection onto the
 * listener direction, divided by 16 and clamped to +-0x7F, sets the pan. */
void SD_CalcSpatialVolumePan(s32 source_x, s32 source_z, u8 *volume, u8 *pan) {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 f;
    s32 g;
    s32 h;
    s32 n;
    s32 q;
    s32 z;
    s32 u;
    s32 v;
    s32 w;

    a = gModel_RView.vpx - source_x;
    b = gModel_RView.vpz - source_z;
    h = SquareRoot0(a * a + b * b);
    q = gModel_RView.vpz;
    c = gModel_RView.vrx * q;
    d = gModel_RView.vrz - q;
    f = gModel_RView.vpx - gModel_RView.vrx;
    g = c - gModel_RView.vpx * gModel_RView.vrz;
    n = SquareRoot0(d * d + f * f);
    h = h - 0x2BC;

    *volume = 0xFF;
    *pan = 0;

    if (h > 0) {
        v = 0xFF - h / 12;
        if (v < 0x10) {
            v = 0x10;
        }
        *volume = v;
    }

    if (n != 0) {
        h = (source_x * d + source_z * f + g) / n;
        w = h / 16;
        v = w;
        w = v < 0;
        if (w) {
            v = -v;
        }
        if (v >= 0x80) {
            v = 0x7F;
        }
        w = v;
        if (h <= 0) {
            w = -w;
        }
        *pan = w;
    }
}

#include "model.h"
#include "model_is_camera_move_complete.h"
#include "model_camera_state.h"

int Model_IsCameraMoveComplete(int arg)
{
    int result = 0;
    if (gModel_CameraMove.flags == 0) {
        if (arg != 0 || gModel_CameraMove.mode == 0 || gModel_CameraMove.follow_countdown == 0)
            result = 1;
    }
    return result;
}

#include "graphics_frame.h"
#include "gpu_packets.h"
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_has_insufficient_buffer_space.h"

/* A small global src/game/model.c owns, declared here as a common symbol the
 * way the original read it through $gp. */
u8 gGraphics_bPacketBufferIndex;

#define MODEL_SLOT_AT_OFFSET(slots, offset) \
    ((ModelSlot *)((u8 *)(slots) + (offset)))

s32 Model_HasInsufficientBufferSpace(s32 index, s32 type) {
    s32 used;
    s32 room;
    ModelSlot *record;
    s32 size;

    used = (s32)gGraphics_pPacketWorkCursor - (s32)gGraphics_abPacketBuffers;
    used -= gGraphics_bPacketBufferIndex * GRAPHICS_PACKET_BUFFER_SIZE;
    room = GRAPHICS_PACKET_BUFFER_SIZE - used;
    /* Typed indexing changes the accepted address construction. */
    record = MODEL_SLOT_AT_OFFSET(gModel_aSlots, index * MODEL_SLOT_SIZE);
    if (type < 0) {
        type = Model_GetSlotTintDescriptor(index)[3];
    }
    size = record->packet_budget_bytes;
    if (type == 5) {
        room -= size * 2;
    } else {
        room -= size;
    }
    return room < MODEL_DATA_MIN_FREE_BYTES;
}
