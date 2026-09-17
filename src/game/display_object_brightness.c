#include "../types.h"
#include "display_object.h"
#include "display_object_brightness.h"

void func_80030090(void)
{
    ((u8 *)&D_800EB184[0]->field_0C)[0] =
        ((u8 *)&D_800EB184[0]->field_0C)[1] =
        ((u8 *)&D_800EB184[0]->field_0C)[2] = 0x40;
}

void func_800300AC(void)
{
    ((u8 *)&D_800EB184[0]->field_0C)[0] =
        ((u8 *)&D_800EB184[0]->field_0C)[1] =
        ((u8 *)&D_800EB184[0]->field_0C)[2] = 0x80;
}
