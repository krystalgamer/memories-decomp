#include "../types.h"

extern u8 D_8009B2B2;
extern u8 D_8009B2B4;
extern u8 D_8009B2B5;
extern u8 D_8009B2B6;
extern u8 D_8009B2B8;
extern u8 D_8009B2C0;
extern u8 D_8009B2C1;
extern u8 D_8009B2C2;
extern u8 D_8009B2DC;
extern u8 D_8009B2E0;
extern void *D_8009B2E4;
extern u8 D_8009B2E9;
extern u8 D_8009B2EA;
extern u8 D_8009B2EB;
extern void *D_8009B2EC;
extern u8 D_8009B2F0;

extern void func_80015A00(void);
extern void func_800300C8(void);
extern void func_8003B6AC(s32, s32);
extern void func_80039A14(void *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800427DC(void *, s32);
extern u8 *TextBox_Create(s32, s32, s32, s32, s32, s32);

void func_80030198(void)
{
    u8 *text_box;
    u8 *object;

    D_8009B2B2 = 0x80;
    D_8009B2EB = 0;
    D_8009B2F0 = 0;
    func_8003B6AC(1, 1);
    text_box = TextBox_Create(1, 15, 16, 16, 0x120, 0xA0);
    text_box[0x5A] = 16;
    text_box[0x5B] = 16;
    func_80039A14(text_box);
    object = func_800400AC(func_8004002C(), 4);
    D_8009B2E4 = object;
    func_800427DC(object, 0);
    object[0x45] = 0xC0;
    object[0x3D] = 0xC0;
    object[0x35] = 0xC0;
    object[0x2D] = 0xC0;
    func_800300C8();
    func_80015A00();
}

void func_80030250(
    void *data,
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
