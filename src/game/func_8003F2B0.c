#include "../types.h"

typedef struct {
    u8 pad0000[0x30];
    s16 field_0030;
    s16 field_0032;
    u8 pad0034[0x2C];
    s16 field_0060;
    u8 pad0062[0x0A];
    u8 field_006C;
} ObjectState;

extern u8 D_800EB0F8[];

extern s32 func_80042B98(ObjectState *, s32, s32, s32);
extern void func_80043178(ObjectState *);
extern void func_80043230(ObjectState *, s32, s32);
extern void TextBox_SetPos(u8 *, s32, s32);

u8 func_8003F2B0(ObjectState *object, s32 arg1, s32 arg2, s32 index)
{
    s32 saved_index = index;
    register s32 value asm("$7");

    if (func_80042B98(object, arg1, arg2, index) == 0) {
        func_80043178(object);
    }

    value = object->field_0060;
    if (value < 0) {
        value += 0x40;
        if (value >= 0) {
            object->field_006C = 0;
            value = 0;
        }
    } else {
        value -= 0x40;
        if (value <= 0) {
            object->field_006C = 0;
            value = 0;
        }
    }
    object->field_0060 = value;

    func_80043230(object, arg1, arg2);

    if (saved_index >= 0) {
        TextBox_SetPos(
            D_800EB0F8 + saved_index * 100,
            object->field_0030,
            object->field_0032);
    }

    return object->field_006C;
}
