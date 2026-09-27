#include "../types.h"
#include "duel_effect_noop_handlers.h"
#ifdef VERSION_EUROPE
#include "../psyq/rand.h"
#include "duel_effect.h"
#include "duel_effect_state_latch.h"
#include "data_transfer_request.h"
#include "mem_card.h"
#include "save_data.h"
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800289AC)
void func_800289AC(void)
{
}
#endif

#if !defined(VERSION_EUROPE) || defined(VERSION_EUROPE_FUNC_800289B4)
#ifdef VERSION_EUROPE
/* European only: this last duel-effect state handler is not empty. Once,
   it stores (rand() << 8) + 1 as the save's duelist code, sets D_8009B3D4
   and requests a save write; then, while MemCardDialog_Poll returns
   nonzero, it sets bit 0x40 of gDuel_bEffectState. */
void func_800289B4(void)
{
    if (DuelEffect_MarkStateInitialized() == 0) {
        ((SaveDataState *)gDuel_awPlayerDeck)->duelist_code = (rand() << 8) + 1;
        D_8009B3D4 = 1;
        SaveData_RequestWrite();
    }
    if (MemCardDialog_Poll() != 0) {
        gDuel_bEffectState |= 0x40;
    }
}
#else
void func_800289B4(void)
{
}
#endif
#endif
