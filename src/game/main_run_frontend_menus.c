#include "../types.h"
#include "../overlays/main_menu/entrypoints.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "fade.h"
#include "file_transfer.h"
#include "sound.h"

extern unsigned char D_8009B26C;
extern unsigned char D_8009B268;
extern unsigned char D_8009B269;
extern unsigned char D_8009B26D;
extern jmp_buf D_800E9DC0;
extern void func_80039E9C(void);
extern void func_8002D458(int);
extern void NameEntry_Init(void);
extern int NameEntry_PollCompletion(void);
extern void func_8003BEB8(void);
extern void func_8003C2B4(void);
extern void Options_Init(void);
extern int Options_Update(void);
extern void func_8003C498(void);
extern void func_8003C950(void);
extern int func_8003CA5C(void);

void Main_RunMenu(void){unsigned char f=D_8009B26C;int r;if((f&0x40)==0){D_8009B26C=f|0x40;File_RequestMainMenuPackage();File_WaitForTransfers();func_80039E9C();MainMenu_InitFrontendMenu(D_8009B268,D_8009B26D);func_80015A00();}rand();r=MainMenu_UpdateFrontendMenu();if(r>=0){SD_BGMFadeOut();Fade_WaitOut();MainMenu_DestroyFrontendMenu();func_8002D458(r);D_8009B269=8;}}

void Main_RunNameEntry(void)
{
    unsigned char flags = D_8009B26C;
    unsigned char value;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestNameEntryPackage();
        NameEntry_Init();
    }
    if (NameEntry_PollCompletion()) {
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}

void Main_RunPasswordMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003BEB8();
        Password_InitShopScreen();
    }
    Password_UpdateShopScreen();
}

void Main_RunOptionsMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C2B4();
        Options_Init();
        func_80015A00();
    }
    if (Options_Update() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}

void Main_RunGameOver(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8003C498();func_8003C950();}if(func_8003CA5C()==0){unsigned char v;SD_BGMFadeOut();Fade_WaitOut();v=D_8009B269;__asm__ volatile("nop");D_8009B26C=v;if(v){D_8009B268=1;D_8009B26D=0;D_8009B26C=8;longjmp(D_800E9DC0,1);}}}

void func_8002D7C4(void)
{
}
