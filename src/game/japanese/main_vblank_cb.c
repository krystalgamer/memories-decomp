#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_MAIN_VBLANK_CB
#define D_8009AF0C D_8009AE4C
#define D_8009B09C gJapanese_FrameCounter
#define D_8009B0C4 gJapanese_D_8009B0C4
#define D_8009B0C8 gJapanese_D_8009B0C8
#define Input_ReadRawPads func_8003C274
#include "../main_frame.c"
