#include "../types.h"
#include "func_80016784.h"

typedef struct {
    u8 pad_00[0x30];
    s16 field_30;
    s16 field_32;
} Object;

void func_80016D04(Object *object, int arg1)
{
    func_80016784(object, arg1, object->field_30, object->field_32);
}
