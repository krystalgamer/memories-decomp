#include "../types.h"
#include "model_scene_setup.h"

extern int D_80010000[3];
extern void func_800533D8(void);
extern void func_80056250(int, int, int, int);

void func_8001755C(void)
{
    func_800530C4();
    func_800533D8();
    func_80056250(2, D_80010000[0], 0x63000, 4);
}
