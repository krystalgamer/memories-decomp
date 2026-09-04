#include "../types.h"

typedef struct {
    u8 pad_00[0x08];
    u16 flags;
    u8 pad_0A[0x17];
    u8 current;
    u8 mode;
    u8 pad_23;
    void *callback;
    s16 x;
    s16 y;
    s16 denominator;
    s16 field_2E;
    s32 old_position;
    u8 pad_34[0x2C];
    s16 speed;
    u8 pad_62[0x0A];
    u8 active;
} DisplayObjectMotion;

extern s32 func_80042B98(DisplayObjectMotion *);
extern void func_80043178(DisplayObjectMotion *);
extern void func_8004318C(DisplayObjectMotion *, s32, s32, s32);

void func_8001EC70(DisplayObjectMotion *object)
{
    if (!func_80042B98(object)) {
        func_80043178(object);
        object->speed = 0;
        object->field_2E = 0;
    }
    func_8004318C(object, object->x, object->y, object->speed);
    object->speed += 0x800 / object->denominator;
    if (object->speed >= 0x800) {
        object->active = 0;
        object->callback = 0;
    }
}

void func_8001ED20(DisplayObjectMotion *object)
{
    if (!func_80042B98(object)) {
        func_80043178(object);
        object->speed = 0;
        object->field_2E = 0;
    }
    if (object->mode) {
        object->mode += 0x40 / object->denominator;
    }
    func_8004318C(object, object->x, object->y, object->speed);
    object->speed += 0x800 / object->denominator;
    if (object->speed >= 0x800) {
        object->old_position = *(s32 *)((u8 *)object + 0x28);
        object->mode = 0;
        if (!object->current) {
            object->flags &= ~4;
        }
        object->active = 0;
        object->callback = 0;
    }
}
