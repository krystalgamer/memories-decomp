#include "../types.h"
#include "duel_effect_request.h"
#include "duel_action_lock.h"
#include "file_transfer.h"
#include "func_8002622C.h"
#include "sound.h"

extern s16 D_8009B1A0;
extern int Duel_CheckRitual(int, int);
extern void func_80029164(int, int);

void func_8002622C(void)
{
    if (!DuelEffect_MarkInitialized()) {
        D_8009B1A0 = Duel_CheckRitual(0, D_8009B1D2);
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
        D_8009B220 = 0;
    }
}
