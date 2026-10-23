#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"
#include "debug_menu_mapped_mode.h"
#include "../psyq/setjmp.h"
#include "display_object_core.h"
#include "fade.h"
#include "duel_effect_reset_state.h"
#include "main_services.h"
#include "debug_menu_exit.h"
#include "../unmatched.h"
#include "main_mode_state.h"
#define GFREEDUEL_BRETURNFLAGS_IN_DATA
#include "../overlays/free_duel/free_duel.h"

void DebugMenu_EnterMappedMode(void)
{
    u8 *p = gDebugMenu_abMainModeByEntry;
    s32 i = gDebugMenu_bCursor;
    u8 v;

    gFreeDuel_bReturnFlags = 0;
    v = p[i];
    D_8009B2B2 = 0;
    gMain_bActiveModeState = v;
}

/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void DebugMenu_Exit(void)
{
    Fade_WaitInitOut();
    DisplayObject_Reset();
    DuelEffect_ResetState();
    longjmp(gMain_RecoveryJmpBuf, 2);
}

#ifdef VERSION_EUROPE
/* A European-only debug-menu handler, which follows DebugMenu_Exit in the
   image and has its own slot in the menu's handler table: it sets both
   gMain_bActiveModeState and gMain_bReturnMode to 15. */
void func_800310FC(void)
{
    gMain_bActiveModeState = 15;
    gMain_bReturnMode = 15;
}
#endif
