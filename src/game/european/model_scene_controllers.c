#include "../../types.h"
#define FILE_TRANSFER_WORDS_IN_DATA
#ifndef EUROPEAN_LANGUAGE_BUILD
#define MODEL_HANDLER_DIAGNOSTICS_AS_ARRAY
#define MODEL_HANDLER_VIEW_OFFSET_ABSOLUTE
#define MODEL_HANDLER_OFFSET_ABSOLUTE
#define MODEL_GRAPHICS_STATE_SCENE_BYTES
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
u16 gModel_wViewYaw;
u16 gModel_wViewPitch;
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

/* SLES-03947..51 build of src/game/model_scene_controllers.c: the whole unit
 * in one object. The symbols below sit at other addresses in the European
 * executables and their US names are taken there, so they are aliased
 * (config/sles_03947/symbols.txt has the addresses). */
#define VERSION_EUROPE
#define gModel_apfnEffectHandlers D_80011414
#define D_8009B004 D_8009BF8C
/* As in model_debug_controller's wrapper: the splat names of the European
 * addresses. */
#define func_801807B0 func_80180420
#define func_80181C4C func_80180004
#define func_80180A24 func_8018019C

/* Values that differ in the European release; the US source names each
 * with an #ifndef default. */
#define MODEL_INTRO_PACKAGE_START_SECTOR 0x6E7
#define MODEL_TEXTURE_RESET_ROW_X 0x280

#include "../model_scene_controllers.c"
