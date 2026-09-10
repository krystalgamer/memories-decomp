#include "../types.h"
#include "func_8001755C.h"
#include "model_slot_setup.h"
#include "model_scene_setup.h"

extern int D_80010000[3];

void func_8001755C(void)
{
    func_800530C4();
    func_800533D8();
    func_80056250(2, (u8 *)D_80010000[0], 0x63000, 4);
}
