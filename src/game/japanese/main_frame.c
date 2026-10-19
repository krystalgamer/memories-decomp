#include "../../types.h"

#define VERSION_JAPAN
#define D_8009AF0C D_8009AE4C
#define D_8009B09C gJapanese_FrameCounter
#define gSaveData_dwVBlankCounter gJapanese_SaveDataVBlankCounter
#define gGraphics_dwSyncVBlankCounter gJapanese_GraphicsSyncVBlankCounter
#include "../main_frame.c"
