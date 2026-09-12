#include "../types.h"
#include "duel_effect_state_latch.h"
#include "graphics_frame.h"
#include "func_80029574.h"
#include "input.h"
#include "fade.h"

#include "display_object_layout.h"
#include "duel_effect.h"
#include "duel_card_viewer.h"
#include "duel_effect_tables.h"
#include "text_box_lifecycle.h"
#include "sound.h"
#include "display_object_api.h"
#include "display_object.h"
#include "display_object_helpers.h"
#include "func_800291E0.h"
#include "duel_effect_resource_setup.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "file_transfer.h"
#include "text_box_runtime.h"
#include "duel_card.h"
#include "duel_effect_resource_record.h"
#include "card_constants.h"
#include "duel_effect_card_viewer_state.h"

extern u8 gDuel_bEffectHandlerFlags;
extern u8 gDuel_bActiveEffectState;

s32 DuelEffect_UpdateState(void) {
    u8 v = gDuel_bEffectState;
    if (v == 0) return 0;
    if ((v & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        gDuel_bActiveEffectState = v;
        gDuel_bEffectState = v | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        gDuel_bEffectHandlerFlags = 0;
        return 1;
    }
    if (v & DUEL_EFFECT_STATE_FLAG_COMPLETE) {
        gDuel_bEffectState = 0;
        return 0;
    }
    gDuelEffect_apfnStateHandler[gDuel_bActiveEffectState]();
    return 1;
}
