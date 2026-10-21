#include "../../types.h"

/* SLES-03947 build of src/game/main_mode_runners.c. */
#define VERSION_EUROPE
#define Options_Init PalOptions_Init
#define Options_Update PalOptions_Update
#define OPTIONS_INIT_ARGS s32 slot
#define MAIN_RUN_OPTIONS_INIT() Options_Init(0)
#define MAIN_RUN_OPTIONS_IS_DONE(result) ((result) >= 0)
#define MAIN_RUN_OPTIONS_FINISH()
#define MAIN_RUN_TRADE_TEXT_BOX_HEIGHT 0x28
#define MAIN_RUN_TRADE_TEXT_BOX_FLAGS 0x40
#define MAIN_RUN_TRADE_SCREEN_HEIGHT 0x100
#define MAIN_RUN_TRADE_TEXT_BOX_Y_OFFSET 0x28
#define MainMenu_InitTradeScreen func_801820C0
#define MainMenu_UpdateTradeScreen func_80182408
#define MainMenu_ReleaseTradeDisplayHandles func_80184210
#include "../main_mode_runners.c"
