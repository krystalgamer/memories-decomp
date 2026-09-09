#include "../types.h"
#include "duel_action_lock.h"
#include "duel_effect_request.h"
#include "sound.h"

void func_80025EE0(void)
{
    DuelEffectRequest *object;

    if (DuelEffect_MarkInitialized() == 0) {
        object = func_8002C68C(0x12);
        object->field_00 = 0xA0;
        object->field_02 = 0x78;
        object->field_1A = 1;
        SD_SEPlayFull(2);
    } else {
        D_8009B220 = 0;
    }
}
