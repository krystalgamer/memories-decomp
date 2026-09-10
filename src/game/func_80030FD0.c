#include "../types.h"
#include "../psyq/setjmp.h"
#include "display_object_config.h"
#include "fade.h"
#include "func_80035A64.h"
#include "main_services.h"
#include "func_80030FD0.h"

/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void func_80030FD0(void)
{
    Fade_WaitInitOut();
    func_800403F0();
    func_80035A64();
    longjmp(D_800E9DC0, 2);
}
