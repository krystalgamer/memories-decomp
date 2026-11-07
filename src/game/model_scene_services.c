/* Model scene services: the host-file readers, the texture upload wrappers,
 * the screen-Y and frame-step setters, the all-parts tint request, the slot-0
 * world position, the presentation scene states, the camera move requests and
 * the orbit direction lock. In every executable these are one contiguous run
 * of text (0x8005988C-0x8005A1F4 in the US build); the unit owns no data. */
#include "../types.h"

/* Small globals src/game/model.c owns, declared here as common symbols the
 * way the original read them through $gp. */
u8 gGraphics_FrameStepControl;
u16 gModel_wScreenYOverride;
s8 gModel_bPresentationPhase;
u8 gModel_bSceneMode;
u16 gModel_wPresentationBgmCommand;
u8 gModel_bOrbitDirectionTimer;
s8 gModel_bOrbitDirection;

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

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/memory.h"
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
#include "../unmatched.h"
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
