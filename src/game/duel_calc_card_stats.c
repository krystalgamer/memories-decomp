#include "../types.h"
#include "card_constants.h"

typedef struct { char p[0xE]; s16 a, b, c, d; } Object;

int Duel_CalcCardStats(Object *object)
{
    int low = object->a + object->c + object->d;
    int high;
    if (low < 0) low = 0;
    if (low > CARD_STAT_MAX) low = CARD_STAT_MAX;
    high = object->b + object->c + object->d;
    if (high < 0) high = 0;
    if (high > CARD_STAT_MAX) high = CARD_STAT_MAX;
    return (high << 16) | low;
}
