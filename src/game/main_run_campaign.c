#include "../types.h"

extern u8 D_8009B26C;
extern u8 D_8009B27A __attribute__((section(".data")));
extern u8 D_800EAE98[];
extern void func_8002FD10(s32);
extern void func_8002FFD4(void *);
extern void func_8002FA54(void);
extern void func_8003FF34(void);
extern void Fade_WaitOut(void);

#define D_8009B254 (*(u8 *)0x8009B254)

void Main_RunCampaign(void) {
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8002FD10(D_8009B27A);
        D_8009B254 = 0;
        return;
    }

    func_8002FFD4(D_800EAE98);
    func_8002FA54();
    if (D_8009B26C & 0x40)
        return;
    func_8003FF34();
    Fade_WaitOut();
}
