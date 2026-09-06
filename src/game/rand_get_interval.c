#include "../types.h"
#include "../psyq/rand.h"

int Rand_GetInterval(int divisor) { return rand() % divisor; }
