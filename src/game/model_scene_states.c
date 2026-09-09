#include "../types.h"
#include "model_scene_states.h"
#include "model_slot_properties.h"
#include "model_effect_state.h"
#include "model_scene_setup.h"

extern s8 D_8009AF9A;
extern u16 D_8009AF96;
extern void func_800533D8(void);

void func_80059C18(s32 value)
{
    D_8009AF96 = value;
}

void func_80059C24(void)
{
    func_800533D8();
    func_800530C4();
    D_8009AF94 = 0x13;
    func_8005F3B8(0, 10000, 0xE00, 0, 0);
    func_80059590(0, 5, 0, 0, 0);
    D_8009AF9A = -1;
}

s32 func_80059C88(void)
{
    return D_8009AF9A == -2;
}

void func_80059C9C(void)
{
    func_800533D8();
    func_800530C4();
    D_8009AF94 = 20;
    D_8009AF9A = -1;
}

s32 func_80059CD0(void)
{
    return D_8009AF9A == -2;
}
