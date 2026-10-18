#include "../../types.h"

/* SLPM-86398 build of src/game/model_scene_setup.c: all nine functions in
 * one object. The symbols below sit at other addresses in the Japanese
 * executable, so they are aliased (config/slpm_86398/symbols.txt). */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#define D_800F2B4A D_800F03E8
#define D_8009AF8C D_8009AEF4
#define D_8009AF94 gJapanese_D_8009AF94
#define D_8009AF9B D_8009AF03
#define D_8009AF9C D_8009AF04
#define D_800F5710 D_800F2FB0
#define D_800F5678 D_800F2F18
#define func_80052528 func_80055D7C
#define func_80052694 func_80055EE8
#define func_80052D2C func_80056544
#define Model_ResetEffectKeyQueue func_8004F534
#define Model_GetCurrentEffectKeyAddress func_8004F564
#include "../model_scene_setup.c"
