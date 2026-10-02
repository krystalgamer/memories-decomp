#include "../../types.h"
#include "helpers.h"

s32 func_80168F70(void)
{
    s32 current;
    s32 target;
    s32 direction;

    if ((current = D_80169050) != (target = D_80169072)) {
        if (current < target) {
            if (target - current > 32767) {
                direction = -1;
            } else {
                direction = 1;
            }
        } else {
            if (current - target > 32767) {
                direction = 1;
            } else {
                direction = -1;
            }
        }
        {
            s32 next = D_80169050;
            s32 limit;

            if (direction >= 0) {
                limit = D_80169072;
                next += 4;
                if (limit - next <= 0) {
                    next = limit;
                }
            } else {
                limit = D_80169072;
                next -= 4;
                if (limit - next >= 0) {
                    next = limit;
                }
            }
            D_80169050 = next;
        }
    }
    return D_80169050 == D_80169072;
}
