#include "../types.h"
#include "input.h"

extern u8 D_8009B0C0, D_8009B39C, D_8009B254, D_8009B3A2;
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
    D_8009B39C = INPUT_REPEAT_INITIAL_DELAY;
    D_8009B254 = 0;
    D_8009B3A2 = INPUT_REPEAT_INTERVAL;
}
