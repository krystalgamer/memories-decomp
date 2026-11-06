#include "../../types.h"
#ifndef EUROPEAN_LANGUAGE_BUILD
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "../../psyq/libetc.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../model_image_copy_frames.h"
#include "../model_graphics_state.h"
#include "../model_effect_requests.h"
#include "../model_transfer_flags.h"
#include "../sound_output_state.h"
#include "../../psyq/stdio.h"
#include "../model.h"
#define MODEL_SCENE_CONTROLLERS_REGIONAL_STORAGE
/* This build's own zero-initialized storage at 0x8009C3D8-0x8009C418 (#2602); its
 * layout differs from the North American block the shared source defines. */
s16 gModel_ImageCopyRectX;
s16 gModel_ImageCopyRectY;
s16 gModel_ImageCopyRectW;
s16 gModel_ImageCopyRectH;
s16 gModel_sTextureTintRectX;
s16 gModel_sTextureTintRectY;
s16 gModel_sTextureTintRectW;
s16 gModel_sTextureTintRectH;
u16 gModel_wViewDistance;
s16 gModel_wViewYaw;
s16 gModel_wViewPitch;
u16 D_8009C3EE_sles_03947;
ModelBytes8 gModel_ViewSnapshot;
s16 gModel_awSlotModelIds[3];
u8 gModelDebug_abSlotAfFlags[2];
u8 gModelDebug_abSlotGfFlags[2];
u16 D_8009C402_sles_03947;
u32 D_8009C404_sles_03947;
u32 D_8009C408;
u32 D_8009C40C_sles_03947;
u32 D_8009C410_sles_03947;
u32 D_8009C414_sles_03947;
#endif

/* SLES-03947..51 build of src/game/model.c. VERSION_EUROPE selects the
 * European scene-controller and slot-control paths. */
#define VERSION_EUROPE

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#define BACKGROUND_RENDER_CLIP_LIMIT 257
#define MODEL_RENDER_BLEND_MODE_5_WORD 0x3C280000
#define MODEL_RENDER_EUROPEAN_TEXTURE_LAYOUT 1
#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x5D4
#define MODEL_AUX_FILE_START_SECTOR 0x2A8
#define MODEL_TEXTURE_STAGE3_RECT_X(index) 0x280
#define MODEL_TEXTURE_STAGE3_RECT_Y(index) (((index) << 4) + 0xD8)
#define MODEL_TEXTURE_PALETTE_RECT_X 0x280
#define MODEL_TEXTURE_TAIL_ROW_Y 0xE0

/* The European executables keep a different object (libpress data) at the US
 * debug-state word's address, so each region's symbols.txt binds D_8009B004 at
 * its own address. */
#include "../model.c"
