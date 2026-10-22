#include "../../types.h"
#include "helpers.h"

s32 func_80168F70(void)
{
    s32 next;
    s32 direction;
    s32 difference;

    if (gPalOptions_wStripScroll != D_80169072) {
        /* The unoptimized counterpart retains this unused frame store. */
        difference = D_80169072 - gPalOptions_wStripScroll;
        if (gPalOptions_wStripScroll < D_80169072) {
            direction = 1;
            if (D_80169072 - gPalOptions_wStripScroll > 32767) {
                direction = -1;
            }
        } else {
            direction = -1;
            if (gPalOptions_wStripScroll - D_80169072 > 32767) {
                direction = 1;
            }
        }
        next = gPalOptions_wStripScroll;
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
        gPalOptions_wStripScroll = next;
    }
    return gPalOptions_wStripScroll == D_80169072;
}
