#include "../../types.h"

#define SD_ARM_BUSY_SDVALUE_ADDRESS 0x8009C7E0
#define gFile_SecondaryPayloadCompleteCallback (*(void (**)(void))0x8009C470)

#include "../sd_arm_busy_callback.c"
