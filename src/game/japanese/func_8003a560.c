#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_FUNC_8003A560
#define DISPLAY_EFFECT_SLOT_ID(i) (((s8 *)D_80010000)[0x7FFF0 + (i)])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 15177
#define D_8009B326 D_8009B216
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#define func_8003A01C DisplayEffect_LoadResourceStage
#define StoreImage func_8007E5C8
#define DisplayEffect_BuildResourceObjects func_8003986C
#define func_8003A560 func_80039BE0
#include "../display_effect_resource_setup.c"
