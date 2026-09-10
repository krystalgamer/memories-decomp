#include "../types.h"
#include "func_80035598.h"

int func_80035598(const u32 *left, const u32 *right)
{
    if (*left == *right) {
        return 0;
    }
    if (*left >= *right) {
        return 1;
    }
    return -1;
}
