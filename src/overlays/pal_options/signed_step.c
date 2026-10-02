#include "../../types.h"
#include "helpers.h"

s32 func_80168F70(void)
{
    s32 next;
    s32 direction;
    s32 difference;

    if (D_80169050 != D_80169072) {
        /* The unoptimized counterpart retains this unused frame store. */
        difference = D_80169072 - D_80169050;
        if (D_80169050 < D_80169072) {
            direction = 1;
            if (D_80169072 - D_80169050 > 32767) {
                direction = -1;
            }
        } else {
            direction = -1;
            if (D_80169050 - D_80169072 > 32767) {
                direction = 1;
            }
        }
        next = D_80169050;
        if (direction >= 0) {
            next += 4;
            if (D_80169072 - next <= 0) {
                next = D_80169072;
            }
        } else {
            next -= 4;
            if (D_80169072 - next >= 0) {
                next = D_80169072;
            }
        }
        D_80169050 = next;
    }
    return D_80169050 == D_80169072;
}
