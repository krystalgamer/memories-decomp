#include "../types.h"
#include "card_constants.h"
#include "display_object_layout.h"

typedef struct {
    char pad_00[8];
    u16 flags;
} DisplayObject;

typedef struct {
    char pad_00[0x22];
    u8 flag;
    char pad_23[0x45];
    u8 type;
} CardObject;

extern void *func_8004006C(void);
extern DisplayObject *func_800400AC(void *, int);
extern void func_800404CC();
extern void func_80042918(DisplayObject *);
extern void func_800428EC(DisplayObject *, int);

DisplayObject *func_80018150(int arg0, int arg1)
{
    DisplayObject *object = func_800400AC(func_8004006C(), 2);
    func_800404CC(object, arg0, arg1, 0, 1, 0, 0x1F, 0x100);
    func_80042918(object);
    func_800428EC(object, -2);
    object->flags |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    return object;
}

int func_800181EC(CardObject *object)
{
    int value = 1;
    switch (object->type) {
    case CARD_TYPE_MAGIC:
    case CARD_TYPE_EQUIP:
        value = 2;
        break;
    case CARD_TYPE_TRAP:
        value = 3;
        break;
    case CARD_TYPE_RITUAL:
        value = 4;
        break;
    }
    if (object->flag != 0) value |= 0x80;
    return value;
}
