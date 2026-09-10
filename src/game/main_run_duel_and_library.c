#define D_8009B0C0_IN_DATA
#include "../types.h"
#include "func_800179F4.h"
#include "graphics_frame.h"
#include "func_800339D0.h"
#include "save_data.h"
#include "main_frame.h"
#include "fade.h"
#include "file_transfer.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "sound.h"
#include "sound_pending_entries.h"
#include "sound_voice_selection.h"
#include "library_runtime.h"
#include "duel_scene_update.h"
#include "duel_side_state.h"
#include "../unmatched.h"

extern u8 D_8009B26C;
#include "high_memory_addresses.h"

void Main_RunLibraryMenu(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8002BFCC();Fade_WaitIn();}else{func_8002BAB4();if((D_8009B26C&0x40)==0){D_8009B0C0=0;SD_BGMFadeOut();Fade_WaitOut();func_800134B4();func_8004763C();func_80047AD0(2);func_80012D84(4);File_WaitForTransfers();}}}
