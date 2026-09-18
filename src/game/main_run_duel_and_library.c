#define D_8009B0C0_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "duel_init_scene.h"
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

#include "high_memory_addresses.h"
#include "main_mode_state.h"

void Main_RunLibraryMenu(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8002BFCC();Fade_WaitIn();}else{func_8002BAB4();if((D_8009B26C&0x40)==0){D_8009B0C0=0;SD_BGMFadeOut();Fade_WaitOut();func_800134B4();func_8004763C();func_80047AD0(2);Main_AdvanceFrames(4);File_WaitForTransfers();}}}
