#include "../types.h"
#include "func_80036D3C.h"
#include "card_constants.h"
#include "campaign_flags.h"
#include "display_object_api.h"
#include "duel_effect.h"
#include "duel_effect_object_commands.h"

void func_80038B08(DuelEffectChannel *object)
{
    func_8004036C(object->field_30);
    object->field_30 = 0;
    object->state_51 = 2;
    object->field_62 = 0;
    D_8009B350 = 1;
}
