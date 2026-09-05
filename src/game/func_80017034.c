#include "../types.h"
#include "duel_card.h"

typedef struct { char p[0xC]; s16 result; char pE[8]; u16 flags; } Object;
typedef struct { char p[0xF]; s8 x, y; } Position;
extern Position *D_8009B1B4;
extern u8 D_800907D8[];
extern u16 D_8009B398[5];

int func_80017034(Object *argument)
{
    register Object *object asm("$6") = argument;
    register Position *position asm("$4") = D_8009B1B4;
    register u8 *table asm("$3") = D_800907D8;
    int valid = 1;
    unsigned int value =
        table[position->y * DUEL_FIELD_ROW_SIZE + position->x];
    if (value >= 20) {
        int mask = object->flags & 0x1000;
        valid = (unsigned int)mask < 1;
    }
    if ((D_8009B398[0] & 0x10) && (object->flags & 0x8000) && valid)
        return object->result;
    return 0;
}
