#include "../../types.h"
#include "apv2.h"
extern Apv2CdCommand safechk_commands[];
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
    s32 i;
    s32 j;
    u8 wait;
    s32 a;
    s32 b;
    u8 c;

    wait = safechk_commands[n].wait;
    *safechk_cdregs = 1;
    b = *D_80169460 & 7;
    if (b == (*D_80169460 & 7)) {
        a = b;
        c = a;
        if (c != 0) {
            D_80169478 += c;
            *safechk_cdregs = 1;
            *D_80169460 = 7;
            for (i = 0; i < 4; i++) {
                *(volatile s32 *)0 = i;
            }
            if (D_80169478 >= wait) {
                D_80169478 = 0;
                if (a == 5) {
                    safechk_results[0] = *D_80169458;
                    safechk_results[1] = *D_80169458;
                    *safechk_cdregs = 1;
                    *D_8016945C = 0x1F;
                    if (!(safechk_results[0] & 0x10)) {
                        return -1;
                    }
                    return -2;
                }
                j = 0;
                if (safechk_commands[n].nresults != 0) {
                    do {
                        safechk_results[j] = *D_80169458;
                        j++;
                    } while (j < safechk_commands[n].nresults);
                }
                *safechk_cdregs = 1;
                *D_8016945C = 0x1F;
                if (n == 10) {
                    return 1;
                }
                if (!(safechk_results[0] & 0x10)) {
                    return 1;
                }
                return -2;
            }
        }
    }
    return 0;
}
