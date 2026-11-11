#include "../types.h"
#include "display_object_system.h"
#include "main.h"
#include "duel_effect_text.h"
#include "input.h"
#include "main_reset_frontend_runtime.h"
#include "main_services.h"
#include "duel_effect_states.h"

void Main_ResetFrontendRuntime(void)
{
    gGraphics_bSyncVBlankTarget = 0;
    DisplayObject_Reset();
    DuelEffect_ResetState();
    DisplayEffect_ResetMenuRecords();
    Main_ClearFrameServiceCallbacks();
    gInput_bRepeatDelay = INPUT_REPEAT_THRESHOLD;
    gDuel_bEffectState = 0;
    gInput_bRepeatInterval = INPUT_REPEAT_RELOAD_VALUE;
}
