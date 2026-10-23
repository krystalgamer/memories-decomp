#include "../../types.h"

#define VERSION_JAPAN
#define DISPLAY_EFFECT_SLOT_ID(i) (((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + (i)])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 15177
#define D_8009B0F4_abs gJapanese_FileTransferFlags
#define D_8009B134_abs gJapanese_FileSecondaryRequest
#include "../display_effect_resource_setup.c"
