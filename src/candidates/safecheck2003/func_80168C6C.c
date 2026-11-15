#include "../../types.h"
#include "safecheck2003.h"

extern SafechkCdCommand safechk_commands[];
/* D_8016941D is the nparams byte of safechk_commands[0]. */
extern u8 D_8016941D[];
/* D_80169458, D_8016945C and D_80169460 are safechk_cdregs[1..3]; indexing
 * the array instead changes the object's relocation addends. */
extern volatile u8 *safechk_cdregs;
extern volatile u8 *D_80169458;
extern volatile u8 *D_8016945C;
extern volatile u8 *D_80169460;
extern u8 safechk_params[];

void func_80168C6C(s32 n)
{
    s32 i;
    s32 k;

    *safechk_cdregs = 1;
    *D_80169460 = 7;
    for (i = 0; i < 4; i++) {
        *(volatile s32 *)0 = i;
    }
    k = n * 4;
    *safechk_cdregs = 1;
    *D_8016945C = 0x18;
    *safechk_cdregs = 0;
    i = 0;
    if (D_8016941D[k] != 0) {
        do {
            *D_8016945C = safechk_params[i];
            i++;
        } while (i < D_8016941D[k]);
    }
    *safechk_cdregs = 0;
    *D_80169458 = safechk_commands[n].cmd;
}
