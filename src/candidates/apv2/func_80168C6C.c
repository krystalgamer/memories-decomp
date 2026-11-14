#include "../../types.h"
#include "apv2.h"

extern Apv2CdCommand D_8016941C[];
extern u8 D_8016941D[];
extern volatile u8 *D_80169454;
extern volatile u8 *D_80169458;
extern volatile u8 *D_8016945C;
extern volatile u8 *D_80169460;
extern u8 D_80169464[];

void func_80168C6C(s32 n)
{
    s32 i;
    s32 k;

    *D_80169454 = 1;
    *D_80169460 = 7;
    for (i = 0; i < 4; i++) {
        *(volatile s32 *)0 = i;
    }
    k = n * 4;
    *D_80169454 = 1;
    *D_8016945C = 0x18;
    *D_80169454 = 0;
    i = 0;
    if (D_8016941D[k] != 0) {
        do {
            *D_8016945C = D_80169464[i];
            i++;
        } while (i < D_8016941D[k]);
    }
    *D_80169454 = 0;
    *D_80169458 = D_8016941C[n].cmd;
}
