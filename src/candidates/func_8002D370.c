/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_run_selection_menus.c.
 */
#include "../types.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "../game/func_800339D0.h"
#include "../game/file_transfer.h"
#include "../game/save_data.h"
#include "../game/main_frame.h"
#include "../game/main_init_free_duel_menu.h"
#include "../game/main_services.h"
#include "../game/main_modes.h"
#include "../game/fade.h"
#include "../game/sound.h"
#define D_8009B26C_AS_SCALAR
#define D_8009B2F8_AS_ARRAY
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "../game/high_memory_addresses.h"

void Main_RunBuildDeckMenu(void){unsigned char flags=D_8009B26C;if((flags&0x40)==0){D_8009B26C=flags|0x40;func_800323F8((u32)D_80010000[0].payload_bases[0],(unsigned char*)gDuel_awPlayerDeck,0,D_8009B2F8[0]);Fade_WaitIn();}else if(func_80033BE8()==0){unsigned char value;SD_BGMFadeOut();Fade_WaitOut();value=D_8009B269;__asm__ volatile("nop");D_8009B26C=value;}}

