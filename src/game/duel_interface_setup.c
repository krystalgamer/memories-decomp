#include "../types.h"
#include "display_object_api.h"
#include "duel_interface_setup.h"
#include "duel_effect.h"
#include "fade.h"
#include "func_800300C8.h"
#include "func_8003B6AC.h"
#include "text_box_lifecycle.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "../unmatched.h"

void func_80030198(void)
{
    DuelEffectChannel *text_box;
    DisplayObject *object;

    D_8009B2B2 = 0x80;
    D_8009B2EB = 0;
    D_8009B2F0 = 0;
    func_8003B6AC(1, 1);
    text_box = TextBox_Create(1, 15, 16, 16, 0x120, 0xA0);
    text_box->field_5A = 16;
    text_box->field_5B = 16;
    func_80039A14((u8 *)text_box);
    object = func_800400AC(func_8004002C(), 4);
    D_8009B2E4 = object;
    func_800427DC(object, 0);
    ((u8 *)&object->field_44)[1] = 0xC0;
    ((u8 *)&object->field_3C)[1] = 0xC0;
    ((u8 *)&object->field_34)[1] = 0xC0;
    ((u8 *)&object->field_2C)[1] = 0xC0;
    func_800300C8();
    Fade_WaitIn();
}

void func_80030250(
    u8 *data,
    s32 field_B4,
    s32 field_B5,
    s32 field_B6,
    s32 field_B8,
    s32 field_C0,
    s32 field_E0
)
{
    D_8009B2EA = 0;
    D_8009B2EC = data;
    D_8009B2B4 = field_B4;
    D_8009B2B5 = field_B5;
    D_8009B2B6 = field_B6;
    D_8009B2E9 = 0;
    D_8009B2DC = 0;
    D_8009B2B8 = field_B8;
    D_8009B2C2 = field_C0;
    D_8009B2C1 = field_C0;
    D_8009B2C0 = field_C0;
    D_8009B2E0 = field_E0;
}
