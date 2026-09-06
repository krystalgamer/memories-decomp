#include "../types.h"
#include "duel_effect.h"

typedef void (*VoidFn)(void);

extern u16 D_8009B2A4;
extern s32 D_800EB12C[];
extern u8 *D_8009B290;
extern u8 D_801A8000[];
extern u16 D_8009B28C;
extern u16 D_8009B27C;
extern VoidFn D_80090C50[];

extern s32 rand(void);
extern void func_80039794(void);

void func_8002FA54(void)
{
    u16 f;
    s32 g;
    s32 b;

    rand();
    f = D_8009B2A4;
    if (f & DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE) {
        func_80039794();
        if ((D_800EB12C[0] & 0x2008) == TEXT_BOX_FLAG_DONE) {
            D_8009B2A4 &= ~DUEL_EVENT_SCRIPT_FLAG_DIALOG_ACTIVE;
        }
        return;
    }
    if (f == 0) {
        return;
    }
    if ((f & DUEL_EVENT_SCRIPT_FLAG_STARTED) == 0) {
        g = D_8009B2A4;
        D_8009B290 = D_801A8000 + *(u16 *)(D_801A8000 + g * 2);
        b = *D_8009B290++;
        D_8009B28C = 0;
        D_8009B2A4 = g | DUEL_EVENT_SCRIPT_FLAG_STARTED;
        D_8009B27C = b;
    }
    if (D_8009B28C != 0) {
        D_8009B27C = D_8009B28C;
    }
    while (1) {
        D_80090C50[D_8009B27C & 0x1F]();
        if (D_8009B27C != 0) {
            return;
        }
        D_8009B27C = *D_8009B290++;
    }
}
