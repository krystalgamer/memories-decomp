#include "../../types.h"
#include "alternate_location.h"

#if defined(VERSION_JAPAN)
#define ALTERNATE_EXIT_CANCEL_BUTTON 0x40
#define ALTERNATE_EXIT_CONFIRM_BUTTONS 0xA0
#else
#define ALTERNATE_EXIT_CANCEL_BUTTON 0x20
#define ALTERNATE_EXIT_CONFIRM_BUTTONS 0xC0
#endif

s32 CampaignMap_PickAlternateExit(void)
{
    s32 i;
    s32 r;
    AlternateLocation *p;
    AlternateExit *q;

    p = &gCampaignMapAlt_aLocationTable[gCampaignMapAlt_Location];
    q = p->exits;
    if (gCampaignMapAlt_Location >= 10) {
        if (func_8004EB9C(71) != 0) {
            if ((D_800C4E68 & ALTERNATE_EXIT_CANCEL_BUTTON) != 0) {
                func_80065B24(48);
                gCampaignMapAlt_MoveState = 24;
                return 0;
            }
        }
    }
    if ((D_800C4E68 & ALTERNATE_EXIT_CONFIRM_BUTTONS) != 0) {
        if ((r = p->f0) != 0) {
            if (func_8004EB9C(q->f0) != 0) {
                r = 0;
            }
        }
        if (r == 0) {
            if (p->f16 != 0) {
                gCampaignMapAlt_MoveState = 24;
                func_80065B24(48);
                return p->f16;
            }
            func_80065B24(48);
            return gCampaignMapAlt_Location | 0x8000;
        }
    }
    i = 0;
    for (; i < 4; q++, i++) {
        if (q->f9 != 16) {
            if (q->f0 != 0 && func_8004EB9C(q->f0) == 0) {
                continue;
            }
            if ((D_800C4E74 & q->f6) != 0) {
                gCampaignMapAlt_MoveState = q->f10;
                func_80065B24(6);
                return q->f9;
            }
        }
        /* No goto targets this label. At -O0 a statement label is what
           emits the nop retail has between the if's end and the for's
           continue point (the q++/i++ that `continue` reaches). */
    next:
        ;
    }
    return -1;
}
