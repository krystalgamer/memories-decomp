#include "../types.h"

extern u8 D_8009B2EB;
extern u16 gDebug_nSceneOrSoundID;
extern u8 D_80090D44[];
extern u8 D_8009B254[];
extern u8 D_8009B3ED[];
extern u8 D_8009B3EA[];
extern u8 D_8009B26C[];
extern u16 D_8009B36A[];
extern u8 D_8009B368[];
extern void func_80030250(void *, int, int, int, int, int, int);
extern int func_80030294(void);
extern int DuelEffect_UpdateState(void);
extern void func_8003594C(int);
extern int func_8003FCD8(void);
extern void func_8005B85C(void);
extern void func_800137E4(void);
extern void func_80024DC8(int, int, int, int);
extern void func_80033C90(void);

void func_80030D5C(void)
{
    {
        u8 flags = D_8009B2EB;

        if ((flags & 0x80) == 0) {
            D_8009B2EB = flags | 0x80;
            gDebug_nSceneOrSoundID = 0;
            func_80030250(D_80090D44, 0x1D, 0, 0, 5, 2, 1);
        }
    }
    {
        u8 flags = D_8009B2EB;

        if (flags & 0x40) {
            register u32 word asm("$2");

            __asm__ volatile(
                "lui $2,%%hi(D_8009B0F4)\n\t"
                "lw $2,%%lo(D_8009B0F4)($2)"
                : "=r"(word)
                :
                : "memory"
            );
            if ((word & 0x02000000) == 0)
                D_8009B2EB = flags & 0xBF;
        } else {
            int result = func_80030294();

            if (result) {
                if (result < 0)
                    D_8009B2EB = 0;
                else {
                    func_8003594C(gDebug_nSceneOrSoundID);
                    D_8009B2EB |= 0x40;
                }
            }
        }
    }
}

void func_80030E30(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254[0] = 3;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void func_80030E7C(void)
{
    u8 flags = D_8009B2EB;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B254[0] = 4;
    }
    if (DuelEffect_UpdateState() == 0) {
        D_8009B2EB = 0;
    }
}

void func_80030EC8(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        D_8009B3ED[0] = 0;
        D_8009B3EA[0] = 0;
    }
    result = func_8003FCD8();
    if (result != 0) {
        if (result == 1) {
            func_8005B85C();
            func_800137E4();
            D_8009B26C[0] = 14;
        }
        D_8009B2EB = 0;
    }
}

void func_80030F40(void)
{
    D_8009B36A[0] = 0x71D0;
    D_8009B368[0] = 0;
    func_80024DC8(-1, 1, 0x8000, 0x8000);
}

void func_80030F80(void)
{
    func_80033C90();
}
