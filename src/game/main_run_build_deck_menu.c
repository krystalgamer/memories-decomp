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
#include "../unmatched.h"

u8 D_8009B26C;
extern u8 D_8009B269;
extern unsigned char D_8009B2F8[];
#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "../game/high_memory_addresses.h"

void Main_RunBuildDeckMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_800323F8(
            (u32)D_80010000[0].payload_bases[0],
            (unsigned char *)gDuel_awPlayerDeck,
            0,
            D_8009B2F8[0]);
        Fade_WaitIn();
    } else if (func_80033BE8() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        Fade_WaitOut();
        value = D_8009B269;
        D_8009B26C = value;
    }
}
