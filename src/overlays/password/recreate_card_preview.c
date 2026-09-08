#include "../../types.h"
#include "shop.h"

extern void func_80029528(s32);
extern PasswordCardPreviewView *func_800291E0(s32, s32, s32);

void Password_RecreateCardPreview(s32 ignored)
{
    PasswordCardPreviewView *obj;

    func_80029528(0);
    obj = func_800291E0(0, -1, -1);
    obj->y = 0x1E;
    obj->phase = 0x80;
    obj->flags |= 0x4;
    D_8016D4D8 = obj;
}
