#include "../types.h"
#include "../psyq/setjmp.h"
#include "display_object_core.h"
#include "fade.h"
#include "func_80035A64.h"
#include "main_services.h"
#include "debug_menu_exit.h"

/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void DebugMenu_Exit(void)
{
    Fade_WaitInitOut();
    func_800403F0();
    func_80035A64();
    longjmp(D_800E9DC0, 2);
}
