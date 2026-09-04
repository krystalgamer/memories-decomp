#include "../types.h"

typedef struct {
    void *value;
    u8 pad4[0x12];
    u16 flags;
} Object;

extern void func_8004036C(void *);

void func_80024914(Object *object)
{
    object->flags &= 0x7FFF;
    if (object->value != 0) {
        func_8004036C(object->value);
        object->value = 0;
    }
}

void func_80024954(Object *object)
{
    func_80024914(object);
    object->flags = 0;
}
