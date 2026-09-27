#include "../../types.h"

/* SLES-03947 build of src/game/main_mode_runners.c: only the function enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_MAIN_RUN_OPTIONS_MENU

#define Main_RunOptionsMenu func_8002D758
#define File_RequestOptionsPackage func_8003C70C
#define Options_Init func_801686AC
#define Options_Update func_80168E1C
#define OPTIONS_INIT_ARGS s32 slot
#define MAIN_RUN_OPTIONS_INIT() Options_Init(0)
#define MAIN_RUN_OPTIONS_IS_DONE(result) ((result) >= 0)
#define MAIN_RUN_OPTIONS_FINISH()

#include "../main_mode_runners.c"
