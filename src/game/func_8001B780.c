#include "../types.h"
#include "display_object.h"

typedef struct {
    unsigned char pad0[4];
    DisplayObject *inner;
    unsigned char pad8[6];
    s8 fieldE;
} Object;

void func_8001B780(Object *object)
{
    DisplayObject *inner = object->inner;

    *(s16 *)&inner->field_30.h.field_30 = object->fieldE * 60 + 14;
    *(s16 *)&inner->field_30.h.field_32 = 194;
}
