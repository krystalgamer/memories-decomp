#ifndef MEMORIES_DECOMP_MAIN_MODES_H
#define MEMORIES_DECOMP_MAIN_MODES_H

#include "../ygo_types.h"

#define MAIN_MODE_COUNT 17

typedef void (*MainModeRunner)(void);

/* The mode runners Main_Loop dispatches, in the order gMain_apfnModeRunner
   holds them; the low five bits of the frontend state byte select one.
   src/game/main_modes.c owns the table at 0x80090B64. */
void Main_RunDebugMenu(void);
void Main_RunAnimatedBattle(void);
void Main_RunCampaign(void);
void Main_RunDuel(void);
void Main_RunLibraryMenu(void);
void Main_RunCampaignMap(void);
void Main_RunFreeDuelMenu(void);
void Main_RunBuildDeckMenu(void);
void Main_RunMenu(void);
void Main_RunNameEntry(void);
void Main_RunPasswordMenu(void);
void Main_RunOptionsMenu(void);
void Main_RunGameOver(void);
void func_8002D7C4(void);
void Main_RunTrade(void);
void Main_RunCredits(void);
void func_8002DC38(void);

extern MainModeRunner gMain_apfnModeRunner[MAIN_MODE_COUNT];

#endif
