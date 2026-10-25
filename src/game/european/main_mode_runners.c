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
#define MAIN_RUN_CREDITS_GEOM_OFFSET_Y 0x80
#define MAIN_RUN_CREDITS_GEOM_SCREEN 0x12C
#define MAIN_RUN_CREDITS_TEXT_BOX_Y 0x10
#define MAIN_RUN_CREDITS_TEXT_BOX_FLAGS 0x12
#define MAIN_RUN_CREDITS_COMPLETION_FLAG 0x10
#define MAIN_RUN_CREDITS_EUROPE_SCENE_CALLBACKS
#define MainMenu_StartValueSetup func_80181050
#define MainMenu_UpdateValueSetup func_80181408
#define MainMenu_FinishValueSetup func_80181F88
#define TWO_PLAYER_SETUP_BOX_X 0x20
#define TWO_PLAYER_SETUP_BOX_Y 0xC4
#define TWO_PLAYER_SETUP_BOX_WIDTH 0x100
#define TWO_PLAYER_SETUP_BOX_HEIGHT 0x24
#define TWO_PLAYER_SETUP_BOX_FLAGS 0x40
#define TWO_PLAYER_SETUP_SECOND_BOX_X 0x18
#define TWO_PLAYER_SETUP_SECOND_BOX_FLAGS 2
#include "../main_mode_runners.c"
