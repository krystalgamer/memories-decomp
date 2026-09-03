#include "../types.h"

extern u8 D_8009B26C, D_8009B26E;
extern u8 D_8009B369[9], D_8009B2F8[9], D_8009B0A3[9];
extern s8 gDuel_bOpponentID[9];
extern u8 D_8009B368[9], D_8009B362[9], D_8009B27A[9];
extern u8 D_8009B370[9];
extern u16 D_8009B16C[9];
extern u32 D_80010000[];
extern u8 gDuel_awPlayerDeck[];
extern void func_800323F8(u32, void *, int, int);
extern void func_80015A00(void), func_8003FF34(void), Fade_WaitOut(void);
extern int func_80033BE8(void);
extern void func_8002CD8C(void), func_800179F4(void), func_80024388(void);
extern void func_80047AD0(int), func_800134B4(void), func_80012D84(int);
extern void func_800137E4(void);

void Main_RunDuel(void)
{
    u8 value = D_8009B26C;
    int state;

    if (!(value & 0x40)) {
        D_8009B26C = value | 0x40;
        D_8009B26E = 1;
        if (!D_8009B369[0] && gDuel_bOpponentID[0] >= 0)
            D_8009B26E = 0;
        D_8009B0A3[0] = 10;
        return;
    }

    value = D_8009B26E;
    state = value & 0xF;
    switch (state) {
    case 0:
        if (!(value & 0x80)) {
            D_8009B26E = value | 0x80;
            D_8009B2F8[0] = 0x80;
            func_800323F8(D_80010000[0], gDuel_awPlayerDeck, 0, 0x80);
            func_80015A00();
        } else if (func_80033BE8() == 0) {
            func_8003FF34();
            Fade_WaitOut();
            func_8002CD8C();
            D_8009B26E = 1;
        }
        break;
    case 1:
        if (!(value & 0x80)) {
            D_8009B26E = value | 0x80;
            func_800179F4();
        } else {
            func_80024388();
            if (D_8009B16C[0] & 0x2000)
                D_8009B26E = 2;
        }
        break;
    case 2:
    {
        u8 *table = D_8009B370;
        u8 next;

        Fade_WaitOut();
        func_8003FF34();
        func_80047AD0(2);
        func_800134B4();
        D_8009B0A3[0] = 6;
        func_80012D84(4);
        func_800137E4();
        next = D_8009B368[0];
        __asm__ volatile("nop");
        D_8009B26C = next;
        if (D_8009B26C == state)
            D_8009B27A[0] = table[D_8009B362[0] * 2];
        break;
    }
    }
}

extern unsigned char D_8009B0C0[];
extern void func_8002BFCC(void),func_8002BAB4(void),func_8004763C(void);

void Main_RunLibraryMenu(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8002BFCC();func_80015A00();}else{func_8002BAB4();if((D_8009B26C&0x40)==0){D_8009B0C0[0]=0;func_8003FF34();Fade_WaitOut();func_800134B4();func_8004763C();func_80047AD0(2);func_80012D84(4);func_800137E4();}}}
