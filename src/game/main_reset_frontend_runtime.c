#include "../types.h"
#include "input.h"
#include "main_reset_frontend_runtime.h"

extern u8 D_8009B0C0, D_8009B254;
extern void func_800403F0(void);
extern void func_80035A64(void);
extern void func_80039E9C(void);
extern void func_800134B4(void);

void Main_ResetFrontendRuntime(void)
{
    D_8009B0C0 = 0;
    func_800403F0();
    func_80035A64();
    func_80039E9C();
    func_800134B4();
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    D_8009B254 = 0;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
}
