#include "../types.h"

extern int rand(void);
int Rand_GetInterval(int divisor) { return rand() % divisor; }
