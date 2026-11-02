#include "../../types.h"

#define SD_ARM_BUSY_SDVALUE_ADDRESS 0x8009B34C
#define gFile_SecondaryPayloadCompleteCallback (*(void (**)(void))0x8009B018)
#include "../sd_arm_busy_callback.c"
