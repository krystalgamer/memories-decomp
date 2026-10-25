#include "../../types.h"

/* SLPM-86398 build of src/game/model_scene_controllers.c: the whole unit in
 * one object. The symbols below sit at other addresses in the Japanese
 * executable (config/slpm_86398/symbols.txt). */
#define VERSION_JAPAN
/* The Japanese camera move record has no field_06 (model.h). */
#define MODEL_CAMERA_MOVE_COMPACT_LAYOUT
#define D_8009AFA4 gJapanese_D_8009AFA4
#define D_8009AFFC D_8009AF5C
#define D_8009B004 gJapanese_D_8009B004
#define D_8009B005 gJapanese_D_8009B005
#define D_800F3A5F gJapanese_D_800F3A5F
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define D_800F2B4A D_800F03E8
#define func_801807B0 func_80180820
#define func_80180A24 func_80180900
#define func_80181C4C func_80181B88
#include "../model_scene_controllers.c"
