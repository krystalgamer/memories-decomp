#include "../types.h"

typedef struct {
    u8 pad0[0x19];
    u8 field19;
} Object;

extern void func_80023D08(Object *);
extern void func_80023FBC(Object *);

int func_80024060(Object *object)
{
    func_80023FBC(object);
    return object->field19;
}

int func_80024088(Object *object)
{
    func_80023D08(object);
    return object->field19;
}
