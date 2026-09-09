#include "../types.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "func_80035A64.h"

extern jmp_buf D_800E9DC0;
extern void func_800403F0(void);
/* The imported prototype lacks noreturn; retail has no epilogue after this call. */

void func_80030FD0(void)
{
    func_80015AD8();
    func_800403F0();
    func_80035A64();
    longjmp(D_800E9DC0, 2);
}
