#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "../overlays/free_duel/free_duel.h"
#include "../overlays/overworld/campaign_map.h"
#include "build_deck_init_screen.h"
#include "build_deck_update.h"
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
 * final gMain_bActiveModeState store. c_symbols.ld supplies the retail storage address. */
u8 gMain_bActiveModeState;

void Main_RunBuildDeckMenu(void)
{
    unsigned char flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        BuildDeck_InitScreen(
            (BuildDeckTransitionState *)D_80010000[0].payload_bases[0],
            (unsigned char *)gDuel_awPlayerDeck,
            0,
            gBuildDeck_bConfirmMode[0]
        );
        Fade_WaitIn();
    } else if (BuildDeck_Update() == 0) {
        unsigned char value;

        SD_BGMFadeOut();
        Fade_WaitOut();
        value = gMain_bReturnMode;
        gMain_bActiveModeState = value;
    }
}
