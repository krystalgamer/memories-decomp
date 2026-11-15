#include "../../types.h"
#include "safecheck2003.h"
extern SafechkCdCommand safechk_commands[];
/* D_80169458, D_8016945C and D_80169460 are safechk_cdregs[1..3]; indexing
 * the array instead changes the object's relocation addends. */
extern volatile u8 *safechk_cdregs;
extern volatile u8 *D_80169458;
extern volatile u8 *D_8016945C;
extern volatile u8 *D_80169460;
extern u8 safechk_results[];
extern s32 D_80169478;
s32 func_80168A98(s32 n)
{
    s32 wait;
    s32 stat;
    s32 i;
    s32 t;
    u8 c;

    wait = safechk_commands[n].wait;
    *safechk_cdregs = 1;
    /* reorg predicts a jump out of a loop as not taken, so the return 0
     * branch fills its delay slot from the fall-through, as retail does. */
    do {
        t = *D_80169460 & 7;
        if (t != (*D_80169460 & 7)) {
            return 0;
        }
    } while (0);
    stat = t;
    c = stat;
    if (c != 0) {
        D_80169478 += c;
        *safechk_cdregs = 1;
        *D_80169460 = 7;
        for (i = 0; i < 4; i++) {
            *(volatile s32 *)0 = i;
        }
        if (D_80169478 < wait) {
            return 0;
        }
        D_80169478 = 0;
        if (stat == 5) {
            volatile u8 *src = D_80169458;

            safechk_results[0] = *src;
            safechk_results[1] = *src;
            *safechk_cdregs = 1;
            *D_8016945C = 0x1F;
            if (safechk_results[0] & 0x10) {
                return -2;
            }
            return -1;
        }
        for (t = 0; t < safechk_commands[n].nresults; t++) {
            safechk_results[t] = *D_80169458;
        }
        *safechk_cdregs = 1;
        *D_8016945C = 0x1F;
        if (n == 10) {
            return 1;
        }
        if (safechk_results[0] & 0x10) {
            return -2;
        }
        return 1;
    }
    return 0;
}
