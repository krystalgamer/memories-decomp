#include "../types.h"
#include "../psyq/rand.h"
#include "rand_get_interval.h"

s32 Rand_GetInterval(s32 divisor) { return rand() % divisor; }
