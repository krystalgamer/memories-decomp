#include "../types.h"
#include "duel_check_ritual.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "file_transfer.h"
#include "duel_ritual_effect.h"
#include "sound.h"
#include "duel_effect_resource_setup.h"
#include "../unmatched.h"
extern s16 D_8009B1A0;

void DuelEffect_StartRitual(void)
{
    if (!DuelEffect_MarkInitialized()) {
        D_8009B1A0 = Duel_CheckRitual(0, gDuel_wEffectCardID);
        if (D_8009B1A0) {
            DuelEffectRequest *request = func_8002C68C(0x12);

            request->field_00 = 0xA0;
            request->field_02 = 0x78;
            request->field_1A = 1;
            SD_SEPlayFull(2);
            func_80029164(1, D_8009B1A0);
        }
    } else if (!(
        (D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
        D_8009B134_abs
    )) {
        gDuel_wCardEffectFlags = 0;
    }
}
