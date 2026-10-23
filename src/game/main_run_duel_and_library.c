#define D_8009B0C0_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "duel_init_scene.h"
#include "graphics_frame.h"
#include "build_deck_update.h"
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

void Main_RunLibraryMenu(void){unsigned char f=gMain_bActiveModeState;if((f&0x40)==0){gMain_bActiveModeState=f|0x40;Library_InitializeScreen();Fade_WaitIn();}else{Library_UpdateScreen();if((gMain_bActiveModeState&0x40)==0){gGraphics_bSyncVBlankTarget=0;SD_BGMFadeOut();Fade_WaitOut();Main_ClearFrameServiceCallbacks();SD_ResetPendingEntries();SD_ReplaceValueLink(2);Main_AdvanceFrames(4);File_WaitForTransfers();}}}
