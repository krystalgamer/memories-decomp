#include "../../types.h"

/* SLPM-86398 build of src/game/model_scene_controllers.c: the whole unit in
 * one object. The symbols below sit at other addresses in the Japanese
 * executable (config/slpm_86398/symbols.txt). */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#define D_8009AFA0 gJapanese_D_8009AFA0
#define D_8009AFA4 gJapanese_D_8009AFA4
#define D_8009AFFC D_8009AF5C
#define D_8009B004 gJapanese_D_8009B004
#define D_8009B005 gJapanese_D_8009B005
#define D_800F3A5F gJapanese_D_800F3A5F
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_800F2B4A D_800F03E8
#define D_8009AF8C D_8009AEF4
#define D_8009AF94 gJapanese_D_8009AF94
#define D_8009AF9B D_8009AF03
#define D_8009AF9C D_8009AF04
#define D_800F5710 D_800F2FB0
#define D_800F5678 D_800F2F18
#define func_80050584 func_80053E40
#define func_80050F24 func_800547E0
#define func_80052528 func_80055D7C
#define func_80052694 func_80055EE8
#define Model_AdvanceSlotLoad func_80059688
#define Model_SetCameraTargetSlots func_8005CE40
#define Model_ResetEffectKeyQueue func_8004F534
#define Model_GetCurrentEffectKeyAddress func_8004F564
#define func_8005F180 func_8004EBD0
#define func_8005F1A4 ModelEffect_GetCoefficient
#define func_8005F27C func_8004ECCC
#define func_8005F588 func_8004EFD8
#define func_8005F5C8 func_8004F018
#define func_801807B0 func_80180820
#define func_80180A24 func_80180900
#define func_80181C4C func_80181B88
#include "../model_scene_controllers.c"
