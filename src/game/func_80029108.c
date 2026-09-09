#include "../types.h"

typedef struct {
    u8 pad_00[0x69];
    u8 field_69;
    u8 field_6A;
} Object;

extern int D_8009B424;
extern void func_80040410(Object *, int);
/* One argument on purpose: func_80041D60 takes three, and reads the
 * second and third on its cooldown path without assigning them. The
 * retail image calls it this way; see that file. */
extern void func_80041D60(Object *);

void func_80029108(Object *object, int arg1)
{
    int value = 2;

    if (arg1 >= 0) {
        value = object->field_6A;
    }
    if (value != object->field_69) {
        func_80040410(object, value);
        func_80041D60(object);
        D_8009B424 = 1;
    }
}
