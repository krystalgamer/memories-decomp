#include "../types.h"
#include "../psyq/setjmp.h"

extern jmp_buf D_800E9DC0;
extern void func_80015AD8(void);
extern void func_800403F0(void);
extern void func_80035A64(void);
/* The imported prototype lacks noreturn; retail has no epilogue after this call. */
extern void longjmp(jmp_buf, int) __attribute__((noreturn));

void func_80030FD0(void)
{
    func_80015AD8();
    func_800403F0();
    func_80035A64();
    longjmp(D_800E9DC0, 2);
}
