/* Model scene runtime: the slot playback speed, direction and seek controls,
 * the host-file readers, the texture upload wrappers, the screen-Y and
 * frame-step setters, the all-parts tint request, the slot-0 world position,
 * the presentation scene states, the camera move requests, the orbit
 * direction lock, the camera distance queries, the branching-child search,
 * the slot part speed and root transform updates, the primitive handler
 * remap, the per-slot camera preset yaw, the spatial sound volume and pan,
 * camera move completion and the packet-buffer space check. In every
 * executable these are one contiguous run of text (0x8005969C-0x8005A98C in
 * the US build). */
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"

/* Small globals src/game/model.c owns, declared here as common symbols the
 * way the original read them through $gp. */
u8 gGraphics_FrameStepControl;
u16 gModel_wScreenYOverride;
s8 gModel_bPresentationPhase;
u8 gModel_bSceneMode;
u16 gModel_wPresentationBgmCommand;
u8 gModel_bOrbitDirectionTimer;
s8 gModel_bOrbitDirection;
u8 gGraphics_bPacketBufferIndex;

/* The camera block this unit owns, right after gModel_aSlots; the generated
 * model_scene_runtime_bss_commons subsegment places it. */
GsCOORDUNIT gModel_CameraCoordinateUnit;
GsRVIEW2 gModel_RView;
GsRVIEW2 gModel_SavedRefView;

#include "model_slot_part_animation.h"
#include "model.h"
#include "model_slot_state_updates.h"
#include "model_slot_updates.h"

void Model_SetSlotPlaybackSpeed(s32 slot_index, s32 speed)
{
    ModelSlot *slot = &gModel_aSlots[slot_index];

    if ((u32)(speed - 4) >= 29) {
        speed = 8;
    }
    slot->playback_speed = speed;
    if (slot->playback_state == 0x3E) {
        Model_SetSlotPlaybackDirection(slot_index, 1);
    }
}

void Model_SetSlotPlaybackDirection(s32 slot_index, s32 sign)
{
    ModelSlot *slot;
    s32 playback_state;
    s32 part_speed;
    s32 double_speed;

    {
        ModelSlot *slot_base = gModel_aSlots;

        slot = slot_base + slot_index;
    }
    if (sign > 0)
        goto state_62;
    if (sign >= 0)
        goto state_35;
    slot->playback_state = 60;
    goto update;
state_35:
    slot->playback_state = 35;
    goto update;
state_62:
    slot->playback_state = 62;
update:
    {
        ModelSlot *speed_slot = &gModel_aSlots[slot_index];

        double_speed = speed_slot->playback_speed * 2;
        playback_state = speed_slot->playback_state;
    }
    switch (playback_state) {
    case 35:
        part_speed = 0;
        break;
    case 62:
        part_speed = double_speed;
        break;
    case 60:
        part_speed = -double_speed;
        break;
    default:
        return;
    }
    Model_SetSlotPartSpeeds(slot_index, part_speed);
}

void Model_SeekSlotAnimation(s32 slot_index, s32 animation_row, s32 frame_position)
{
    s32 part_index = 0;
    ModelSlot *slot = &gModel_aSlots[slot_index];
    s32 part_count;
    s32 scaled_position;

    part_count = slot->animation_sequence_count;
    slot->animation_position = frame_position << 4;
    scaled_position = slot->animation_position;

    if (part_count != 0) {
        do {
            s32 selected_row = slot->current_animation_row;

            if (animation_row != 0) {
                ModelSlotPart *part_sequence = slot->animation_sequences[part_index];
                selected_row = animation_row;
                part_sequence->sid = animation_row;
            }

            Model_SeekSlotPartAnimation(slot, part_index, selected_row, scaled_position);
            part_count = slot->animation_sequence_count;
            part_index++;
        } while (part_index < part_count);
    }

    if (animation_row != 0) {
        slot->current_animation_row = animation_row;
    }
    slot->cached_dimensions[3] = 0;
}

#include "../psyq/libsn.h"
#include "file_query_wrappers.h"

int HostFile_GetSize(char *path)
{
    int handle = PCopen(path, 0, 0);
    int size;

    if (handle < 0)
        return -1;
    size = PClseek(handle, 0, 2);
    PCclose(handle);
    return size;
}

s32 HostFile_ReadAll(char *path, char *destination)
{
    return HostFile_ReadRange(path, destination, 0, 0);
}

#define FILE_READ_CHUNK_SIZE 0x1000

s32 HostFile_ReadRange(char *path, char *destination, s32 offset, s32 length)
{
    s32 handle;
    s32 total;
    s32 remaining;
    s32 chunk;

    if (path == 0)
        return 0;
    handle = PCopen(path, 0, 0);
    if (handle < 0)
        return 0;
    if (length == 0)
        length = PClseek(handle, 0, 2) - offset;
    total = length;
    remaining = total;
    PClseek(handle, offset, 0);
    if (total > 0) {
        do {
            chunk = FILE_READ_CHUNK_SIZE;
            if (remaining <= FILE_READ_CHUNK_SIZE)
                chunk = remaining;
            if (PCread(handle, destination, chunk) != chunk) {
                total = 0;
                break;
            }
            remaining -= chunk;
            destination += chunk;
        } while (remaining > 0);
    }
    PCclose(handle);
    return total;
}

#include "model_texture_upload.h"
#include "model.h"
#include "model_set_slot_shadow_enabled.h"

void ModelTexture_UploadTimFile(char *path, int destination_mode, int blend_mode, int pixel_x, int pixel_y, int clut_x, int clut_y)
{
    /* The 32-byte retail scratch is one word larger than GsIMAGE. */
    s32 local[8];

    ModelTexture_LoadTim((GsIMAGE *)local, path, destination_mode, blend_mode, pixel_x, pixel_y, clut_x, clut_y);
}

/* Copies the image header onto the stack before forwarding it, so the
   callee can't alias the caller's copy. */
u32 ModelTexture_PackPageClutCopy(s32 side, s32 blend_mode, GsIMAGE *src) {
    GsIMAGE buf = *src;
    return ModelTexture_PackPageClut(side, blend_mode, &buf);
}

int Model_SetSlotShadowEnabled(int index, int value)
{
    register ModelSlot *entry;
    register int old;

    {
        register ModelSlot *base = gModel_aSlots;

        entry = &((ModelSlot *)0)[index];

        entry = (ModelSlot *)((unsigned int)entry + (unsigned int)base);
    }

    old = entry->shadow_enabled;
    if (value >= 0) {
        entry->shadow_enabled = value;
    }
    return old;
}

#include "model_graphics_state.h"
#include "model_state_setters.h"
#include "graphics_frame_controls.h"

void Model_SetScreenYOverride(int value)
{
    gModel_wScreenYOverride = value;
}

void Model_SetFrameStepOverride(int value)
{
    gGraphics_FrameStepControl = value;
}

#include "model_queue_tint_request.h"

void Model_QueueTintRequestForAllParts(
    s32 slot, s32 mode, ModelTintColor start, ModelTintColor end, s32 duration)
{
    start.blend_mode = mode & 0x7F;
    Model_QueueTintRequest(slot, 0, start, end, duration, 0);
}

#include "model_update_view_metrics.h"
#include "model_view_coordinate_helpers.h"

#define HMD_COORD_UNIT_OFFSET sizeof(SVECTOR)
#define HMD_MATRIX_OFFSET \
    (HMD_COORD_UNIT_OFFSET + sizeof(GsCOORDUNIT))
#define HMD_SCRATCH_SIZE (HMD_MATRIX_OFFSET + sizeof(MATRIX))

void Model_GetSlot0OffsetWorldPosition(s16 value, s16 *out)
{
    /* One block preserves the retail SVECTOR/GsCOORDUNIT/MATRIX stack order. */
    u8 scratch[HMD_SCRATCH_SIZE];
    u16 result;

    memset(scratch, 0, sizeof(SVECTOR));
    ((SVECTOR *)scratch)->vx = value;
    Model_BuildViewRelativeCoordinateUnit(
        0,
        15,
        (SVECTOR *)scratch,
        (GsCOORDUNIT *)(scratch + HMD_COORD_UNIT_OFFSET)
    );
    GsGetLwUnit(
        (GsCOORDUNIT *)(scratch + HMD_COORD_UNIT_OFFSET),
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    );
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[0];
    out[0] = result;
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[1];
    out[1] = result;
    result = *(volatile u16 *)&(
        (MATRIX *)(scratch + HMD_MATRIX_OFFSET)
    )->t[2];
    out[3] = 0;
    out[2] = result;
}

#include "camera_view.h"
#include "model_scene_controllers.h"
#include "model_update_tint_requests.h"
#include "model_slot_control.h"
#include "model_remap_primitive_handlers.h"
#include "model_background.h"
#include "model_cleanup.h"
#include "model_handler_registry.h"
#include "primitive_handler_modes.h"
#include "model_scene_states.h"
#include "model_slot_properties.h"
#include "model_effect_state.h"
#include "model_transfer_state.h"
#include "../game/model_draw_slot.h"

void Model_SetPresentationBgmCommand(s32 value)
{
    gModel_wPresentationBgmCommand = value;
}

void Model_StartExodiaPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    gModel_bSceneMode = 0x13;
    Model_SetCameraOrbit(0, 10000, 0xE00, 0, 0);
    Model_SetSlotTintTarget(0, 5, 0, 0, 0);
    gModel_bPresentationPhase = -1;
}

s32 Model_IsExodiaPresentationComplete(void)
{
    return gModel_bPresentationPhase == -2;
}

void Model_StartCreditsPresentation(void)
{
    Model_ResetSceneState();
    Model_ResetSceneView();
    gModel_bSceneMode = 20;
    gModel_bPresentationPhase = -1;
}

s32 Model_IsCreditsPresentationComplete(void)
{
    return gModel_bPresentationPhase == -2;
}

void Model_UpdateScene(void)
{
    u8 *state = (u8 *)&gModel_RView;

    GsSetRefView2((GsRVIEW2 *)state);
    if (gModel_bSceneMode == 15) {
        Model_UpdateAnimatedBattle();
    }
    if (gModel_bSceneMode == 19) {
        Model_UpdateExodiaPresentation();
    }
    if (gModel_bSceneMode == 20) {
        Model_UpdateCreditsPresentation();
    }
    GsSetRefView2((GsRVIEW2 *)state);
    Model_DrawSlot(0);
    Model_DrawSlot(1);
    Model_DrawSlot(2);
    Model_UpdateSlotControl(0);
    Model_UpdateSlotControl(1);
    Model_DrawBackground();
    Model_StepBackgroundVramAnimation();
    Model_UpdateTintRequests();
    Model_UpdateSlotAnimation(0);
    Model_UpdateSlotAnimation(1);
    Model_UpdateSlotAnimation(2);
    if (!Model_IsEffectKeyQueueEmpty()) {
        Model_UpdateEffectKeyframes();
    }
    Model_UpdateCamera();
}

void Model_RunSlotHandlers(s32 index)
{
    ModelSlot *model_slot = &gModel_aSlots[index];
    ModelSlotHeadEntry *head_unit = model_slot->hmd_units;
    s32 (*remap_handler)(s32) = Model_MapToWireframePrimitiveHandler;
    u32 *group_words;
    u32 *next_group_words;
    s32 unit_index;

    if (model_slot->is_active == 0) {
        return;
    }
    for (unit_index = 0; unit_index < model_slot->hmd_unit_count; unit_index++, head_unit++) {
        group_words = head_unit->primitive_data;
        if (group_words != 0) {
            while (1) {
                if (group_words[2] != 0) {
                    Model_RemapPrimitiveHandlers(
                        remap_handler,
                        (ModelHandlerRunEntry *)&group_words[3],
                        group_words[1],
                        group_words[2]
                    );
                }
                next_group_words = (u32 *)group_words[0];
                if (next_group_words == (u32 *)-1) {
                    break;
                }
                group_words = next_group_words;
            }
        }
    }
}

#include "model_start_camera_move.h"
#include "model_transfer_flags.h"
#include "model_effect_requests.h"
#include "model_camera_state.h"

void Model_RequestCameraMove(s32 mode_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        Model_StartCameraMove(mode_request, 0, 0, 0);
    }
}

void Model_RequestCameraMoveToSlots(s32 mode_request, s32 eye_slot, s32 target_slot, s32 timing_request)
{
    s32 eye_selector = eye_slot < 0 ? 0 : eye_slot + 1;
    s32 target_selector = target_slot < 0 ? 0 : target_slot + 1;
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        Model_StartCameraMove(mode_request, eye_selector, target_selector, timing_request);
    }
}

void Model_SetCameraEyeSlots(s32 pair_slot, s32 slot_index)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->eye.pair_slot = pair_slot;
        camera_move->eye.slot = slot_index;
    }
}

void Model_SetCameraTargetSlots(s32 pair_slot, s32 slot_index)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->target.pair_slot = pair_slot;
        camera_move->target.slot = slot_index;
    }
}

void Model_SetCameraMoveDurationFromMagnitude(s32 magnitude_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state == 1 && Model_GetHandlerCallbackState() == hold_state) {
        return;
    }
    {
        ModelCameraMove *camera_move = &gModel_CameraMove;

        camera_move->duration = (magnitude_request < 0 ? -magnitude_request : magnitude_request) * 2;
        camera_move->elapsed = 0;
    }
}

void Model_SetCameraFollowCountdown(s32 countdown_request)
{
    s32 hold_state = Model_GetCameraHoldState();

    if (hold_state != 1 || Model_GetHandlerCallbackState() != hold_state) {
        gModel_CameraMove.follow_countdown = countdown_request * 2;
    }
}

void Model_SetCameraOrbitStep(s32 orbit_step_request)
{
    s32 hold_state = Model_GetCameraHoldState();
    ModelCameraMove *camera_move;

    if (hold_state == 1) {
        if (Model_GetHandlerCallbackState() == hold_state) {
            return;
        }
    }
    camera_move = &gModel_CameraMove;
#ifndef VERSION_JAPAN
    camera_move->base_orbit_step = orbit_step_request;
#endif
    camera_move->orbit_step = orbit_step_request;
}

#include "model_lock_orbit_direction.h"

void Model_LockOrbitDirection(int value){int one=1;if(Model_GetCameraHoldState()!=one||Model_GetHandlerCallbackState()!=one){if(value>=0)gModel_bOrbitDirection=one;else gModel_bOrbitDirection=-1;gModel_bOrbitDirectionTimer=255;}}

#include "model.h"
#include "camera_view.h"
#include "model_distance_queries.h"

#define MODEL_SLOT_HALFWORDS(slots, index) \
    ((u16 *)((u8 *)(slots) + (index) * MODEL_SLOT_SIZE))

s32 Model_GetSlotDistanceFromCameraEye(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_aSlots[0].cached_position, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = gModel_RView.vpx - entry[0];
    difference.vy = gModel_RView.vpy - entry[1];
    difference.vz = gModel_RView.vpz - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}

s32 Model_GetSlotDistanceFromCameraTarget(s32 index)
{
    SVECTOR output;
    SVECTOR difference;
    u16 *entry = MODEL_SLOT_HALFWORDS(gModel_aSlots[0].cached_position, index);

    memset(&difference, 0, sizeof(difference));
    difference.vx = gModel_RView.vrx - entry[0];
    difference.vy = gModel_RView.vry - entry[1];
    difference.vz = gModel_RView.vrz - entry[2];
    output = difference;
    return SquareRoot0(
        output.vx * output.vx + output.vy * output.vy + output.vz * output.vz
    );
}

#include "model_parent_search.h"

s32 Model_FindBranchingChildCoord(ModelSlot *model, GsCOORDUNIT *parent)
{
    s32 entry_count_snapshot = model->entry_count;
    s32 child_index = 0;
    s32 coordinate_byte_offset;
    s32 grandchild_index;
    s32 entry_count_limit;
    GsCOORDUNIT *child_candidate;
    GsCOORDUNIT *candidate_coordinate;
    GsCOORDUNIT *grandchild_candidate;
    u8 *coordinate_bytes;

    if (entry_count_snapshot == 0)
        goto done;
    coordinate_byte_offset = child_index;
    child_candidate = (GsCOORDUNIT *)model->entries;
    do {
        grandchild_index = 0;
        if (child_candidate->super != parent)
            goto next;
        if (entry_count_snapshot == 0)
            goto next;
        entry_count_limit = entry_count_snapshot;
        coordinate_bytes = model->entries;
        candidate_coordinate = (GsCOORDUNIT *)(coordinate_bytes + coordinate_byte_offset);
        grandchild_candidate = (GsCOORDUNIT *)coordinate_bytes;
inner:
        if (grandchild_candidate->super == candidate_coordinate)
            goto after_inner;
        grandchild_index++;
        if (grandchild_index < entry_count_limit) {
            grandchild_candidate++;
            goto inner;
        }
after_inner:
        if (grandchild_index < model->entry_count)
            goto done;
next:
        coordinate_byte_offset += sizeof(GsCOORDUNIT);
        child_candidate++;
        entry_count_snapshot = model->entry_count;
        child_index++;
    } while (child_index < entry_count_snapshot);
done:
    return child_index;
}

#include "model_slot_updates.h"

#include "model_update_coordinate_unit_matrix.h"

#define MODEL_UNIT_OFFSET(member) ((u32)&(((GsCOORDUNIT *)0)->member))

/* The slot data entries at ModelSlot.entries, the record root_coordinate points at
 * and the block Model_BuildViewRelativeCoordinateUnit builds on the stack are all one type: libhmd's
 * GsCOORDUNIT. These pin every offset the game still reaches by hand. */
extern char ModelSlotDataEntry_must_be_a_GsCOORDUNIT[
    sizeof(GsCOORDUNIT) == MODEL_SLOT_DATA_ENTRY_SIZE ? 1 : -1
];
extern char GsCOORDUNIT_matrix_offset_must_be_0x4[
    MODEL_UNIT_OFFSET(matrix) == 0x4 ? 1 : -1
];
extern char GsCOORDUNIT_matrix_t_offset_must_be_0x18[
    MODEL_UNIT_OFFSET(matrix.t) == 0x18 ? 1 : -1
];
extern char GsCOORDUNIT_workm_offset_must_be_0x24[
    MODEL_UNIT_OFFSET(workm) == 0x24 ? 1 : -1
];
extern char GsCOORDUNIT_rot_offset_must_be_0x44[
    MODEL_UNIT_OFFSET(rot) == 0x44 ? 1 : -1
];
extern char GsCOORDUNIT_super_offset_must_be_0x4C[
    MODEL_UNIT_OFFSET(super) == 0x4C ? 1 : -1
];

#undef MODEL_UNIT_OFFSET

/* ModelSlotPart is model.h's mirror of libhmd's GsSEQ, kept there so that
 * header does not need the libhmd chain. Every field agrees. */
#define MODEL_SEQ_AGREES(member) \
    ((u32)&(((ModelSlotPart *)0)->member) == (u32)&(((GsSEQ *)0)->member))
extern char ModelSlotPart_must_be_a_GsSEQ[
    sizeof(ModelSlotPart) == sizeof(GsSEQ) ? 1 : -1
];
extern char ModelSlotPart_seq_fields_must_agree[
    MODEL_SEQ_AGREES(rewrite_idx) && MODEL_SEQ_AGREES(size) &&
    MODEL_SEQ_AGREES(num) && MODEL_SEQ_AGREES(ii) &&
    MODEL_SEQ_AGREES(aframe) && MODEL_SEQ_AGREES(sid) &&
    MODEL_SEQ_AGREES(speed) && MODEL_SEQ_AGREES(srcii) &&
    MODEL_SEQ_AGREES(rframe) && MODEL_SEQ_AGREES(tframe) &&
    MODEL_SEQ_AGREES(ci) && MODEL_SEQ_AGREES(ti) &&
    MODEL_SEQ_AGREES(start) && MODEL_SEQ_AGREES(start_sid) &&
    MODEL_SEQ_AGREES(traveling) ? 1 : -1
];
#undef MODEL_SEQ_AGREES

void Model_SetSlotPartSpeeds(s32 slot_index, s32 speed_request)
{
    ModelSlot *slot_state = &gModel_aSlots[slot_index];
    ModelSlotPart **part_sequences;
    s32 part_index;

    part_sequences = slot_state->animation_sequences;
    part_index = 0;
    if (slot_state->animation_sequence_count != 0) {
        do {
            (*part_sequences)->speed = speed_request;
            part_sequences++;
            part_index++;
        } while (part_index < slot_state->animation_sequence_count);
    }
}

/* Places the slot's own coordinate unit: translation into the local matrix's
 * t vector and a single yaw into its Euler angles, then rebuilds the matrix.
 * The unit pointer is reloaded per store, as retail does. */
void Model_SetSlotRootTransform(ModelSlot *slot_state, int translation_x, int translation_y, int translation_z, int yaw_request)
{
    if (slot_state->root_coordinate) {
        slot_state->root_coordinate->rot.vx = 0;
        slot_state->root_coordinate->rot.vy = yaw_request;
        slot_state->root_coordinate->rot.vz = 0;
        slot_state->root_coordinate->matrix.t[0] = translation_x;
        slot_state->root_coordinate->matrix.t[1] = translation_y;
        slot_state->root_coordinate->matrix.t[2] = translation_z;
    }
    Model_UpdateCoordinateUnitMatrix(slot_state->root_coordinate, 0);
}

#include "model_handler_registry.h"
#include "model_remap_primitive_handlers.h"

void Model_RemapPrimitiveHandlers(s32 (*remap_handler)(s32), ModelHandlerRunEntry *descriptor, s32 arg2,
                   s32 descriptor_count)
{
    s32 remaining_descriptors;
    s32 type_key;
    u32 metadata;
    u32 count_flags;
    s32 type_tag;
    s32 descriptor_stride_words;
    s32 replacement_handler;

    for (remaining_descriptors = descriptor_count - 1; remaining_descriptors != -1; remaining_descriptors--) {
        type_key = Model_FindHandlerKey(descriptor->handler_value);
        /* Single-assignment metadata/stride/flags in this order keep the
         * first scheduling pass on the retail instruction order. */
        metadata = descriptor->metadata;
        descriptor_stride_words = (metadata & 0xFFFF) + 1;
        type_tag = type_key >> 24;
        count_flags = metadata & 0xFFFF0000;
        if (((u32)type_tag < 2 && (type_key & 0xFFFF) != 0 && count_flags != 0) ||
            (type_tag == 1 && (type_key & 0xFFFF) == 0)) {
            replacement_handler = remap_handler(descriptor->handler_value);
            if (replacement_handler != -1) {
                descriptor->handler_value = replacement_handler;
            }
        }
        descriptor = (ModelHandlerRunEntry *)&((u32 *)descriptor)[descriptor_stride_words];
    }
}

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

#include "main.h"
#include "gpu_packets.h"
#include "model_graphics_state.h"
#include "model_slot_properties.h"
#include "model_has_insufficient_buffer_space.h"

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
