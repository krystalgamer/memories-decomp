#include "../../types.h"
#include "helpers.h"

s32 PalOptions_StepStripScroll(void)
{
    s32 next;
    s32 direction;
    s32 difference;

    if (gPalOptions_wStripScroll != gPalOptions_wStripScrollTarget) {
        /* The unoptimized counterpart retains this unused frame store. */
        difference = gPalOptions_wStripScrollTarget - gPalOptions_wStripScroll;
        if (gPalOptions_wStripScroll < gPalOptions_wStripScrollTarget) {
            direction = 1;
            if (gPalOptions_wStripScrollTarget - gPalOptions_wStripScroll > 32767) {
                direction = -1;
            }
        } else {
            direction = -1;
            if (gPalOptions_wStripScroll - gPalOptions_wStripScrollTarget > 32767) {
                direction = 1;
            }
        }
        next = gPalOptions_wStripScroll;
        if (direction >= 0) {
            next += 4;
            if (gPalOptions_wStripScrollTarget - next <= 0) {
                next = gPalOptions_wStripScrollTarget;
            }
        } else {
            next -= 4;
            if (gPalOptions_wStripScrollTarget - next >= 0) {
                next = gPalOptions_wStripScrollTarget;
            }
        }
        gPalOptions_wStripScroll = next;
    }
    return gPalOptions_wStripScroll == gPalOptions_wStripScrollTarget;
}
