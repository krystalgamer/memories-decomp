#include "../types.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "duel_transition_color.h"
#include "file_transfer.h"
#include "save_data.h"
#include "main_frame.h"
#include "main_init_free_duel_menu.h"
#include "main_services.h"
#include "main_modes.h"
#include "fade.h"
#include "sound.h"
#include "../unmatched.h"

extern u8 D_8009B26C;
extern u8 D_8009B269;
extern unsigned char D_8009B2F8[];
extern u32 D_80010000[];

void Main_RunCampaignMap(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;D_8009B0A3[0]=10;File_RequestEgyptOverworldPackage();func_8016866C(D_8009B363[0]);Fade_StartIn();}else{CampaignMap_UpdateLocation();if((D_8009B26C&0x40)==0){func_800134B4();D_8009B0A3[0]=6;func_80012D4C();}}}

void Main_RunBuildDeckMenu(void){unsigned char flags=D_8009B26C;if((flags&0x40)==0){D_8009B26C=flags|0x40;func_800323F8(D_80010000[0],(unsigned char*)gDuel_awPlayerDeck,0,D_8009B2F8[0]);Fade_WaitIn();}else if(func_80033BE8()==0){unsigned char value;SD_BGMFadeOut();Fade_WaitOut();value=D_8009B269;__asm__ volatile("nop");D_8009B26C=value;}}

void Main_RunFreeDuelMenu(void) {
    unsigned char flags = D_8009B26C;
    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        Main_InitFreeDuelMenu();
        Fade_WaitIn();
    }
    func_80168FB4();
    if ((D_8009B26C & 0x40) == 0) SD_BGMFadeOut();
}
