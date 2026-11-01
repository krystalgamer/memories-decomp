#include "../../types.h"

/* SLPM-86398 build of src/game/graphics_frame.c: the symbols below sit at other addresses in the
 * Japanese executable and their US names are taken there, so they are aliased
 * (config/slpm_86398/symbols.txt has the addresses). The US source is included
 * unchanged. */
#define D_8009AFA4 gJapanese_D_8009AFA4
#define D_8009B0A8 gJapanese_D_8009B0A8
#define VERSION_JAPAN
#define D_8009B0D8 gJapanese_FrameStep
#define gGraphics_dwSyncVBlankCounter gJapanese_GraphicsSyncVBlankCounter
#define gGraphics_bClearBackground gJapanese_D_8009B0D0
#define gSaveData_dwVBlankCounter gJapanese_SaveDataVBlankCounter

#include "../graphics_frame.c"
