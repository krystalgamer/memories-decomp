#include "../types.h"
#include "func_80037C74.h"

s32 func_80037C74(u8 *object) {
    if (*(s16 *)(object + 0x38) >= *(s16 *)(object + 0x3E)) {
        *(s16 *)(object + 0x38) = 0;
        *(u16 *)(object + 0x3A) += object[0x5B];
    }
    if (*(s16 *)(object + 0x42) < *(s16 *)(object + 0x3A) + object[0x5B]) {
        *(u16 *)(object + 0x3A) -= object[0x5B];
        return 1;
    }
    return 0;
}
