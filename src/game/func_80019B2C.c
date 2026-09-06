#include "../types.h"
#include "display_object_layout.h"

typedef struct {
    char p0[8]; u16 flags; char pA[0x17]; u8 current;
    char p22[2]; void *callback; u8 target; char p29; s16 step;
    char p2C[0x40]; u8 active;
} Object;

void func_80019B2C(u8 *arg0) {
    s32 n = arg0[0x21] + arg0[0x2A];
    s32 d;

    arg0[0x21] = n;
    if (*(s16 *)(arg0 + 0x2A) >= 0) {
        d = arg0[0x28] - n;
    } else {
        d = n - arg0[0x28];
    }

    if ((s8)d < 0) {
        u8 c = arg0[0x28];

        arg0[0x6C] = 0;
        *(s32 *)(arg0 + 0x24) = 0;
        arg0[0x21] = c;
        if (c == 0) {
            *(u16 *)(arg0 + 8) &= ~DISPLAY_OBJECT_FLAG_CLIP_TEST;
        }
    }
}
