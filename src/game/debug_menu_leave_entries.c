#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_IN_DATA
#include "../types.h"
#include "frontend_debug_tables.h"
#include "frontend_debug_state.h"
#include "debug_menu_mapped_mode.h"
#include "../psyq/setjmp.h"
#include "display_object_core.h"
#include "fade.h"
#include "func_80035A64.h"
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
    D_8009B26C = v;
}

/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void DebugMenu_Exit(void)
{
    Fade_WaitInitOut();
    DisplayObject_Reset();
    func_80035A64();
    longjmp(D_800E9DC0, 2);
}
