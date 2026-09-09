#include "../types.h"
#include "duel_effect_mode_7.h"
#include "file_transfer.h"
#include "func_8003FCD8.h"
#include "movie_playback_control.h"
#include "../unmatched.h"

/* The frontend's scene states, in address order. Every one of them is a step
   of the same state machine: the flags byte D_8009B2EB carries bit 0x80 for
   "already entered", so the first call does the setup and each later call
   polls for completion and clears the byte on the way out. The eight are
   contiguous and are the whole gcc_2_8_1_g8 run between func_80030998 and
   func_80030FA0. */

extern u8 D_80090D0C[];
extern u8 D_80090D28[];
extern u8 D_80090D44[];
extern u8 D_8009B254[];
extern u8 D_8009B268[];
extern u8 D_8009B26C[];
extern u8 D_8009B26D[];
extern u8 D_8009B2E8;
extern u8 D_8009B2EB;
extern u8 D_8009B363[];
extern u8 D_8009B368[];
extern u16 D_8009B36A[];
extern u8 D_8009B3EA[];
extern u8 D_8009B3ED[];
extern u16 gDebug_nSceneOrSoundID;

extern void func_80030250(void *, int, int, int, int, int, int);
extern int DuelEffect_UpdateState(void);
extern void func_80024DC8(int, int, int, int);
void func_80030C10(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        gDebug_nSceneOrSoundID = D_8009B2E8;
        func_80030250(D_80090D0C, 0x14, 0, 0, 0xC, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B363[0] = 0;
            D_8009B26C[0] = 5;
            D_8009B2E8 = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}

void func_80030CB0(void)
{
    u8 flags = D_8009B2EB;
    int result;

    if ((flags & 0x80) == 0) {
        D_8009B2EB = flags | 0x80;
        gDebug_nSceneOrSoundID = D_8009B26D[0];
        func_80030250(D_80090D28, 0x15, 0, 0, 0x12, 2, 1);
    }
    result = func_80030294();
    if (result != 0) {
        if (result < 0) {
            D_8009B2EB = 0;
        } else {
            D_8009B268[0] = 1;
            D_8009B26C[0] = 8;
            D_8009B26D[0] = *(u8 *)&gDebug_nSceneOrSoundID;
        }
    }
}

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
            File_RequestMainMenuPackage();
            File_WaitForTransfers();
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
