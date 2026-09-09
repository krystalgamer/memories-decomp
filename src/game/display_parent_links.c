#include "../types.h"
#include "duel_grid.h"
#include "display_object.h"

typedef struct DisplayLinkEntry {
    DisplayObject *object;
    u8 pad_04[8];
} DisplayLinkEntry;

typedef struct DisplayParent {
    DisplayObject *position_base;
    DisplayObject *base;
    DisplayLinkEntry *entries;
    u8 pad_0C[0xB];
    u8 index;
} DisplayParent;

extern void func_80022EEC(void);

void func_80022F98(DisplayParent *parent, volatile DisplayObject *object)
{
    DisplayObject *base;
    int index;

    if (object != 0) {
        base = parent->position_base;
        object->position.h.field_28 = object->field_30.h.field_30 - base->field_30.h.field_30;
        object->position.h.field_2A = object->field_30.h.field_32 - base->field_30.h.field_32;
        index = parent->index;
        object->field_6C = 1;
        object->update = (DisplayObjectCallback)func_80022EEC;
        object->field_2C.h.field_2C = index;
    }
}

void func_80022FF0(DisplayParent *argument, int flag)
{
    register DisplayParent *parent asm("$19") = argument;
    register int clear asm("$20") = flag;
    register DisplayLinkEntry *a asm("$17");
    register DisplayLinkEntry *b asm("$16");
    register int i asm("$18");

    a = parent->entries;
    if (a != 0) {
        i = 0;
        b = (DisplayLinkEntry *)((u8 *)a + 4);
        func_80022F98(parent, parent->base);
        parent->base = 0;
        do {
            func_80022F98(parent, a->object);
            func_80022F98(parent, b->object);
            if (clear) {
                a->object = 0;
                b->object = 0;
            }
            b++;
            i++;
            a++;
        } while (i < DUEL_FIELD_ROW_SIZE);
    }
}
