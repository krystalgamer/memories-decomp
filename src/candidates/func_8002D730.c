/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8 this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/main_run_frontend_menus.c.
 */
#include "../types.h"
#include "../overlays/main_menu/entrypoints.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "../overlays/password/shop.h"
#include "../psyq/rand.h"
#include "../psyq/setjmp.h"
#include "../game/fade.h"
#include "../game/file_transfer.h"
#include "../game/main_menu_selection.h"
#include "../game/func_8003C2B4.h"
#include "../game/game_over.h"
#include "../game/main_modes.h"
#include "../game/menu_record_reset.h"
#include "../game/sound.h"
#include "../game/main_services.h"
#include "../game/options.h"

extern u8 D_8009B26C;
extern u8 D_8009B269;

void Main_RunGameOver(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8003C498();func_8003C950();}if(func_8003CA5C()==0){unsigned char v;SD_BGMFadeOut();Fade_WaitOut();v=D_8009B269;__asm__ volatile("nop");D_8009B26C=v;if(v){D_8009B268=1;D_8009B26D=0;D_8009B26C=8;longjmp(D_800E9DC0,1);}}}
