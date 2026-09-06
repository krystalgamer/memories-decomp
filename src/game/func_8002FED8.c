#include "../types.h"
#include "../psyq/libgte.h"

extern u8 *D_800EAE98[];

void func_8002FED8(u8 *state, u8 *color)
{
    s32 angle;
    s32 intensity;
    s32 component;
    u8 *object;

    if ((*(u16 *)(state + 4) & 0x8000) == 0) {
        *(u16 *)(state + 4) |= 0x8000;
        *(s16 *)(state + 6) = 0;
    }
    angle = (*(u16 *)(state + 6) + 32) & (ONE - 1);
    *(u16 *)(state + 6) = angle;
    intensity = rsin(angle) * 24 / ONE;
    if (intensity == 0)
        intensity = 1;
    if (intensity == 24)
        intensity = 23;
    component = intensity - 104;
    color[14] = component;
    color[13] = component;
    color[12] = component;
    object = D_800EAE98[0];
    if (object != (u8 *)0) {
        component = (intensity + 24) / 2 - 128;
        object[14] = component;
        object[13] = component;
        object[12] = component;
    }
    object = D_800EAE98[5];
    if (object != (u8 *)0) {
        component = (intensity + 24) / 2 - 128;
        object[14] = component;
        object[13] = component;
        object[12] = component;
    }
}
