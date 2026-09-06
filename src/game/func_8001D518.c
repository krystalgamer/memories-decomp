#include "../types.h"
#include "display_object_layout.h"

typedef struct { char p[8]; u16 flags; char pA[0x60]; u8 index; } Object;
typedef struct { char p[0xA]; u8 index; } Source;
extern Object *D_8009B1F8;
extern void *func_8004002C(void);
extern Object *func_800400AC(void *, int);
extern void func_80040468();
extern void func_800428EC(Object *, int);
extern void func_80015DB8(Object *);

Object *func_8001D518(Source *source)
{
    Object *object = D_8009B1F8;
    if (object == 0) {
        object = func_800400AC(func_8004002C(), 2);
        func_80040468(object, 4, 3, 8, 0xB, 0x1F0);
        object->index = source->index;
        func_800428EC(object, 1);
        *(void **)((char *)object + 0x24) = func_80015DB8;
        object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    }
    return object;
}
