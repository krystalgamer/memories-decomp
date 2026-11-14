#include "../../types.h"
#include "apv2.h"
extern Apv2CdCommand D_8016941C[];
extern volatile u8 *D_80169454;
extern volatile u8 *D_80169458;
extern volatile u8 *D_8016945C;
extern volatile u8 *D_80169460;
extern u8 D_8016946C[];
extern s32 D_80169478;
s32 func_80168A98(s32 n)
{
    s32 i;
    s32 j;
    u8 wait;
    s32 a;
    s32 b;
    u8 c;

    wait = D_8016941C[n].wait;
    *D_80169454 = 1;
    b = *D_80169460 & 7;
    if (b == (*D_80169460 & 7)) {
        a = b;
        c = a;
        if (c != 0) {
            D_80169478 += c;
            *D_80169454 = 1;
            *D_80169460 = 7;
            for (i = 0; i < 4; i++) {
                *(volatile s32 *)0 = i;
            }
            if (D_80169478 >= wait) {
                D_80169478 = 0;
                if (a == 5) {
                    D_8016946C[0] = *D_80169458;
                    D_8016946C[1] = *D_80169458;
                    *D_80169454 = 1;
                    *D_8016945C = 0x1F;
                    if (!(D_8016946C[0] & 0x10)) {
                        return -1;
                    }
                    return -2;
                }
                j = 0;
                if (D_8016941C[n].nresults != 0) {
                    do {
                        D_8016946C[j] = *D_80169458;
                        j++;
                    } while (j < D_8016941C[n].nresults);
                }
                *D_80169454 = 1;
                *D_8016945C = 0x1F;
                if (n == 10) {
                    return 1;
                }
                if (!(D_8016946C[0] & 0x10)) {
                    return 1;
                }
                return -2;
            }
        }
    }
    return 0;
}
