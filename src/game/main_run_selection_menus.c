#include "../types.h"
#include "duel_transition_color.h"
#include "file_transfer.h"
#include "save_data.h"
#include "main_frame.h"
#include "fade.h"
#include "sound.h"

extern unsigned char D_8009B26C;
extern unsigned char D_8009B269;
extern unsigned char D_8009B0A3[];
extern unsigned char D_8009B363[];
extern unsigned char D_8009B2F8[];
extern unsigned int D_80010000[];
extern void func_8016866C(int);
extern void CampaignMap_UpdateLocation(void);
extern void func_800134B4(void);
extern void func_800323F8(unsigned int, void *, int, int);
extern void Main_InitFreeDuelMenu(void);
extern void func_80168FB4(void);

void Main_RunCampaignMap(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;D_8009B0A3[0]=10;File_RequestEgyptOverworldPackage();func_8016866C(D_8009B363[0]);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((D_8009B26C&0x40)==0){func_800134B4();D_8009B0A3[0]=6;func_80012D4C();}}}

void Main_RunBuildDeckMenu(void){unsigned char flags=D_8009B26C;if((flags&0x40)==0){D_8009B26C=flags|0x40;func_800323F8(D_80010000[0],(unsigned char*)gDuel_awPlayerDeck,0,D_8009B2F8[0]);func_80015A00();}else if(func_80033BE8()==0){unsigned char value;SD_BGMFadeOut();Fade_WaitOut();value=D_8009B269;__asm__ volatile("nop");D_8009B26C=value;}}

void Main_RunFreeDuelMenu(void) {
    unsigned char flags = D_8009B26C;
    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        Main_InitFreeDuelMenu();
        func_80015A00();
    }
    func_80168FB4();
    if ((D_8009B26C & 0x40) == 0) SD_BGMFadeOut();
}
