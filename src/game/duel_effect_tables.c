#include "../types.h"
#include "../unmatched.h"
#include "duel_effect_noop_handlers.h"
#include "duel_effect_tables.h"
#include "func_8002622C.h"
#include "duel_action_lock.h"
#include "duel_card_effects.h"
#include "duel_field_effect_steps.h"
#include "duel_magic_effect_dispatch.h"
#include "func_80025EE0.h"
#include "func_80028310.h"
#include "func_800283F4.h"

/* Initialized data at 0x80090A5C: two handlers per effect group, in group
   order. */
DuelEffectHandler gDuelEffect_apfnGroupHandler
    [DUEL_EFFECT_GROUP_COUNT * DUEL_CARD_EFFECT_HANDLERS_PER_GROUP] = {
    func_80024E4C, func_80024E4C,
    func_80024E4C, func_80024E58,
    func_80024E4C, func_800250C8,
    func_80024E4C, func_8002525C,
    func_80024E4C, func_800257A0,
    func_80024E4C, func_8002538C,
    func_80024E4C, DuelEffect_UpdateFieldMarker,
    func_80024E4C, func_8002596C,
    func_80024E4C, func_80025BEC,
    func_80024E4C, func_80025D30,
    func_80025EE0, func_80025F3C,
    func_80024E4C, func_800260D0,
    func_8002622C, func_800262D4,
    func_80024E4C, func_80026A3C,
    func_80024E4C, func_80024E4C,
};

/* The group each card effect id dispatches to. */
u8 gDuelEffect_abGroupByEffectId[DUEL_EFFECT_ID_COUNT] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x04, 0x07, 0x02, 0x02, 0x02,
    0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x0a,
    0x09, 0x08, 0x00, 0x00, 0x05, 0x00, 0x0b, 0x05,
    0x00, 0x00, 0x00, 0x05, 0x05, 0x05, 0x05, 0x05,
    0x0c, 0x0c, 0x0c, 0x00, 0x09, 0x0c, 0x0c, 0x0d,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
    0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00,
};

/* The five effect-state handlers DuelEffect_UpdateState runs. */
DuelEffectHandler
    gDuelEffect_apfnStateHandler[DUEL_EFFECT_STATE_HANDLER_COUNT] = {
    func_80028310,
    func_80028310,
    func_800283F4,
    func_800289AC,
    func_800289B4,
};
