#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "func_800323F8.h"
#include "func_800339D0.h"
#include "file_transfer.h"
#include "save_data.h"
#include "main_frame.h"
#include "main_init_free_duel_menu.h"
#include "main_services.h"
#include "main_modes.h"
#include "fade.h"
#include "sound.h"
#define D_8009B2F8_AS_ARRAY
#include "../unmatched.h"

#define HIGH_MEMORY_ADDRESSES_MODEL_PREFIX
#include "high_memory_addresses.h"
#include "main_mode_state.h"

/* The common definition makes MASPSX preserve the load-delay nop before the
 * final D_8009B26C store. c_symbols.ld supplies the retail storage address. */
u8 D_8009B26C;

void Main_RunBuildDeckMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_800323F8(
            (u32)D_80010000[0].payload_bases[0],
            (unsigned char *)gDuel_awPlayerDeck,
            0,
            D_8009B2F8[0]
        );
        Fade_WaitIn();
    } else if (func_80033BE8() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        Fade_WaitOut();
        value = D_8009B269;
        D_8009B26C = value;
    }
}
