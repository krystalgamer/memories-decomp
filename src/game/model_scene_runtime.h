#ifndef MEMORIES_DECOMP_MODEL_SCENE_RUNTIME_H
#define MEMORIES_DECOMP_MODEL_SCENE_RUNTIME_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "model.h"

/* The size of host file `path`, found by opening it through the SN fileserver
 * and seeking to its end. Returns -1 when the open fails. */
int HostFile_GetSize(char *path);

/* Reads the whole of host file `path` to `destination` through
 * HostFile_ReadRange and returns the byte count, or 0 on failure.
 * model_texture_upload.c loads a TIM to 0x80400000 with it. */
s32 HostFile_ReadAll(char *path, char *destination);

/* Reads `length` bytes of a host file into memory through the SN fileserver,
   in 0x1000-byte chunks. A zero `length` means "to the end of the file".
   Returns the number of bytes transferred, or 0 if the open failed or a chunk
   came up short. */
s32 HostFile_ReadRange(
    char *path, char *destination, s32 offset, s32 length
);

void Model_UpdateScene(void);
void Model_RunSlotHandlers(s32 index);

s32 Model_GetSlotDistanceFromCameraEye(s32 index);
s32 Model_GetSlotDistanceFromCameraTarget(s32 index);

/* The unmatched model controller and its tracked candidate still relocate
 * against the original address symbol. c_symbols.ld aliases it to the named
 * matching definition above. */

/* Two guarded requests into Model_StartCameraMove.
 *
 * Both consult Model_GetCameraHoldState and Model_GetHandlerCallbackState first and do nothing when the
 * state is 1 and the two agree, so a caller cannot assume the request was
 * issued. Neither reports whether it was.
 *
 * Model_RequestCameraMoveToSlots biases its middle two arguments before passing them on: a
 * negative becomes 0 and anything else is incremented, so the values reaching
 * Model_StartCameraMove are not the ones the caller wrote. */
void Model_RequestCameraMove(s32 value);
void Model_RequestCameraMoveToSlots(s32 first, s32 second, s32 third, s32 fourth);

/* Stores the camera move's two target-slot selectors, under the
 * same Model_GetCameraHoldState/Model_GetHandlerCallbackState guard: nothing is written when the state
 * is 1 and the two agree. */
void Model_SetCameraTargetSlots(s32 first, s32 second);

/* Stores twice `value` into the camera move's follow_countdown, under that same
 * guard -- written with the test inverted, so the store sits inside
 * `state != 1 || Model_GetHandlerCallbackState() != state` rather than after an early return
 * (model_scene_services.c:372).
 *
 * This unit defines seven functions and this header declares the ones with
 * consumers outside it. Model_SetCameraFollowCountdown was the fourth such case and the only
 * one whose caller wrote the prototype by hand: Model_UpdateAnimatedBattle,
 * the model scene mode 15 controller, calls it once with 0x78
 * (src/game/model_scene_controllers.c:413) while already including this header.
 * Its two immediate neighbours in the file, Model_SetCameraMoveDurationFromMagnitude and Model_SetCameraOrbitStep,
 * have no uses outside the unit at all, which is why they are still not
 * declared here. The latter writes both base_orbit_step and orbit_step in the US
 * camera record, but only orbit_step in the compact Japanese record. */
void Model_SetCameraFollowCountdown(s32 value);

s32 Model_GetSlotCameraPresetYaw(s32 index);

/* Reports whether the model buffer lacks room for the record `index` would
 * load, comparing what gGraphics_pPacketWorkCursor has handed out against the shared graphics
 * packet-buffer capacity and the size the slot's own entry asks for. Non-zero
 * means there is not enough. */
s32 Model_HasInsufficientBufferSpace(s32 index, s32 type);

/* The camera-move completion gate at 0x8005A878.
 *
 * It returns nonzero when the camera move record gModel_CameraMove (ModelCameraMove,
 * model.h) has `flags == 0`, and -- only when `arg` is zero -- additionally
 * requires `mode` or `follow_countdown` to be zero. The unit defines this one function
 * and nothing else.
 *
 * Model_UpdateAnimatedBattle, the model scene mode 15 controller
 * (src/game/model_scene_controllers.c), is the only source in the tree that
 * names it, and as a candidate it declared the prototype for itself because
 * this unit had no header to take it from. All twelve call sites pass 1, so there the test
 * reduces to `flags == 0` and the `arg == 0` path has no reader here; that is a
 * statement about this tree, not about the rest of the binary.
 * Follow/orbit motion can continue after flags clear; this is not a physical
 * stillness, effect-key queue, or request-permission query.
 *
 * The declaration is moved verbatim, `int` included: that is the spelling both
 * the definition and the former candidate already used. */
int Model_IsCameraMoveComplete(int arg);

void Model_LockOrbitDirection(int value);

/* Returns the index of the first coordinate entry whose super is `parent`
 * and which is itself the super of some entry -- the first child of
 * `parent` that branches further -- or entry_count when there is none.
 * Model_InitSlot passes two entries from the model's coordinate-unit array
 * as the parent to find. */
s32 Model_FindBranchingChildCoord(ModelSlot *model, GsCOORDUNIT *parent);

/* Fixed prefix of one mapped HMD descriptor. handler_value is the callback
 * word, not the packed registry type key. metadata retains the descriptor
 * word count in its low half and the original count/flags in its high half. */
typedef struct {
    s32 handler_value;
    s32 metadata;
} ModelHandlerRunEntry;

typedef char ModelHandlerRunEntry_prefix_size_must_be_8[
    sizeof(ModelHandlerRunEntry) == 8 ? 1 : -1
];

/* Walks a run of variable-length descriptors, conditionally replacing their
 * callback words. The low halfword at +4 counts words AFTER the leading
 * handler word: stride is (low16 + 1) u32 words, not bytes or a fixed array.
 * The high half remains the raw count/flags gate, not a normalized count.
 *
 * Reverse lookup obtains the type key used for eligibility. The callback
 * takes and returns the stored s32 handler value, not that packed key, an
 * index or a descriptor pointer; only -1 rejects a replacement.
 *
 * arg2 remains the original third argument passed by both callers. The body
 * never reads it; do not remove its call-site setup or infer a new use. */
void Model_RemapPrimitiveHandlers(
    s32 (*fn)(s32),
    ModelHandlerRunEntry *entry,
    s32 arg2,
    s32 count
);

/* Reads model slot `index`'s shadow-enable byte and returns it, replacing it
 * first when `value` is non-negative. model_scene_controllers.c uses both halves:
 * it saves the old value with one call and restores it with another. */
s32 Model_SetSlotShadowEnabled(s32 index, s32 value);

/* Three updates applied to one model slot, all selected by slot index rather
 * than by a slot pointer, so callers do not need the record's layout.
 *
 * Model_SetSlotPlaybackSpeed stores the speed (values outside 4..32 become 8)
 * and, when the slot is already playing forward (state 0x3E), reapplies
 * forward playback so the parts pick up the new speed; it does not start an
 * idle or held slot. Model_SetSlotPlaybackDirection turns a sign into the
 * slot's playback state and part speed;
 * Model_SeekSlotAnimation sets the slot's frame and, when the animation
 * argument is nonzero, its current animation, seeking every part there. */
void Model_SetSlotPlaybackSpeed(s32 slot_index, s32 speed);
void Model_SetSlotPlaybackDirection(s32 slot_index, s32 sign);
void Model_SeekSlotAnimation(s32 slot_index, s32 animation_row, s32 frame_position);

/* The two per-slot update steps model_slot_control.c and
 * Model_SetSlotPlaybackDirection (src/game/model_slot_state_updates.c) drive. Model_SetSlotPartSpeeds sets
 * the playback speed of every part's GsSEQ in one slot; Model_SetSlotRootTransform takes
 * the slot record itself rather than its index, which is why the two do not
 * share a parameter shape. */
void Model_SetSlotPartSpeeds(s32 index, s32 speed);
void Model_SetSlotRootTransform(ModelSlot *record, s32 translation_x, s32 translation_y, s32 translation_z, s32 yaw_request);

/* The unit's two one-line setters, each storing its argument into a single
 * model state global and doing nothing else: Model_SetScreenYOverride writes
 * the halfword at gModel_wScreenYOverride, Model_SetFrameStepOverride the byte at
 * gGraphics_FrameStepControl.
 *
 * Both are declared s32 here, which is the type the definitions already use --
 * src/types.h defines s32 as `signed int`. The store narrows to the global's
 * own width, which is what the matched bytes do: functions.csv records these
 * as the "straight unsigned-halfword G8 setter" and "straight unsigned-byte G8
 * setter", each matching with its GPREL16 relocation.
 * The intro controller calls Model_SetFrameStepOverride before reading its
 * live phase. */
void Model_SetScreenYOverride(s32 value);
void Model_SetFrameStepOverride(s32 value);

/* Distance-to-camera fade and pan for one sound source at (source_x, source_z). The
 * square root of the distance from the listener record at gModel_RView, minus
 * 700, sets the volume byte in *volume (0xFF down to 0x10). The signed
 * projection onto the listener direction, divided by 16 and clamped to
 * +-0x7F, sets the pan in *pan. */
void SD_CalcSpatialVolumePan(
    s32 source_x, s32 source_z, u8 *volume, u8 *pan
);

#endif
