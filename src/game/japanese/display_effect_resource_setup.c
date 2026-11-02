#include "../../types.h"

#define VERSION_JAPAN
#define DISPLAY_EFFECT_SLOT_ID(i) (((s8 *)gRuntime_pSharedWorkBase)[0x7FFF0 + (i)])
#define DISPLAY_EFFECT_RESOURCE_FIRST_SECTOR 15177
#include "../display_effect_resource_setup.c"
