#include "../../types.h"

extern void *D_80184558;
extern void *D_8018455C;
extern void *D_80184560;
extern void *D_80184568[];
extern s32 D_800E9DB0;
extern void func_8004036C(void *);

void func_80180DD0(void)
{
    s32 i;

    func_8004036C(D_80184558);
    D_80184558 = 0;
    func_8004036C(D_8018455C);
    D_8018455C = 0;
    func_8004036C(D_80184560);
    D_80184560 = 0;
    for (i = 0; i < 0xB; i++) {
        if (D_80184568[i] != 0) {
            func_8004036C(D_80184568[i]);
            D_80184568[i] = 0;
        }
    }
    D_800E9DB0 = 0;
}
