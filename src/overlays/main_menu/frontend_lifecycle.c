#include "../../types.h"
#include "entrypoints.h"
#include "frontend.h"

extern void *D_80184558;
extern void *D_8018455C;
extern void *D_80184560;
extern u8 *gMain_apMenuEntries[];
extern u8 D_80184596;
extern u8 D_80184599;
extern s32 D_800E9DB0;
extern void func_8004036C(void *);

void MainMenu_StartFrontendEntryTransition(s32 mode)
{
    s32 i;
    s32 offset;

    for (i = 0; i < 0xB; i++) {
        if (i & 1) {
            offset = 0x1E0;
        } else {
            offset = -0xA0;
        }
        if (gMain_apMenuEntries[i] != 0) {
            if (mode != 0) {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = 0xA0;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = offset;
            } else {
                *(s16 *)(gMain_apMenuEntries[i] + 0x36) = offset;
                *(s16 *)(gMain_apMenuEntries[i] + 0x38) = 0xA0;
            }
            *(s16 *)(gMain_apMenuEntries[i] + 0x30) = *(u16 *)(gMain_apMenuEntries[i] + 0x36);
            *(s16 *)(gMain_apMenuEntries[i] + 0x60) = 0x10;
        }
    }
    D_80184596 = mode;
    D_80184599 = 1;
}

void MainMenu_DestroyFrontendMenu(void)
{
    s32 i;

    func_8004036C(D_80184558);
    D_80184558 = 0;
    func_8004036C(D_8018455C);
    D_8018455C = 0;
    func_8004036C(D_80184560);
    D_80184560 = 0;
    for (i = 0; i < 0xB; i++) {
        if (gMain_apMenuEntries[i] != 0) {
            func_8004036C(gMain_apMenuEntries[i]);
            gMain_apMenuEntries[i] = 0;
        }
    }
    D_800E9DB0 = 0;
}
