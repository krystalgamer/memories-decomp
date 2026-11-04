#include "../../types.h"
#include "helpers.h"

/* Unoptimized duplicate of PalOptions_StepStripScroll (signed_step.c) over its
   own suffix storage. No caller or reference to it is known. */
s32 PalOptions_StepStripScrollCopy(void)
{
    s32 next;
    s32 direction;
    s32 difference;

    if (gPalOptions_wStripScrollCopy != gPalOptions_wStripScrollTargetCopy) {
        /* The unoptimized counterpart retains this unused frame store. */
        difference = gPalOptions_wStripScrollTargetCopy - gPalOptions_wStripScrollCopy;
        if (gPalOptions_wStripScrollCopy < gPalOptions_wStripScrollTargetCopy) {
            direction = 1;
            if (gPalOptions_wStripScrollTargetCopy - gPalOptions_wStripScrollCopy > 32767) {
                direction = -1;
            }
        } else {
            direction = -1;
            if (gPalOptions_wStripScrollCopy - gPalOptions_wStripScrollTargetCopy > 32767) {
                direction = 1;
            }
        }
        next = gPalOptions_wStripScrollCopy;
        if (direction >= 0) {
            next += 4;
            if (gPalOptions_wStripScrollTargetCopy - next <= 0) {
                next = gPalOptions_wStripScrollTargetCopy;
            }
        } else {
            next -= 4;
            if (gPalOptions_wStripScrollTargetCopy - next >= 0) {
                next = gPalOptions_wStripScrollTargetCopy;
            }
        }
        gPalOptions_wStripScrollCopy = next;
    }
    return gPalOptions_wStripScrollCopy == gPalOptions_wStripScrollTargetCopy;
}
