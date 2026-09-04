#include "../types.h"
#include "card_constants.h"

typedef struct { char p[0x22]; u8 flag; char p23[0x45]; u8 type; } Object;

int func_800181EC(Object *object)
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
