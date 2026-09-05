#include "../../types.h"

typedef struct {
    u8 unk0[0x8];
    u16 flags;
    u8 unkA[0x17];
    u8 unk21;
    u8 unk22[0x10];
    u16 unk32;
} PasswordObject;

extern PasswordObject *D_8016D4D8;
extern void func_80029528(s32);
extern PasswordObject *func_800291E0(s32, s32, s32);

void func_8016A02C(void)
{
    PasswordObject *obj;

    func_80029528(0);
    obj = func_800291E0(0, -1, -1);
    obj->unk32 = 0x1E;
    obj->unk21 = 0x80;
    obj->flags |= 0x4;
    D_8016D4D8 = obj;
}
