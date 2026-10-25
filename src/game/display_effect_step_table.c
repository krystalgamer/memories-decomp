#include "../types.h"
#include "display_effect_step_table.h"
#include "display_effect_update_resource_acquisition.h"
#include "dialog_transition.h"
#include "display_effect_lifecycle.h"
#include "display_effect_fade_out_record.h"
#include "display_effect_update_position_tween.h"
#include "display_effect_fade_in_record.h"
#include "display_effect_update_callbacks.h"
#include "dialog_update_message_transition.h"

/* Initialized data at 0x80090F68: the display effect step table.
 *
 * DisplayEffect_ProcessMenuRecords walks the menu records and, for each live
 * one, dispatches `display_effect_step & 0x1F`. The mask permits thirty-two
 * entries and there are thirteen, so an index above twelve would read past
 * the end into the table that follows. That is recorded rather than
 * corrected: nothing here shows the game produces such an index, and widening
 * the array would change the bytes.
 *
 * The table and dispatcher use MenuRecord *, matching the arena they walk.
 * The two repeated lifecycle entries and the single VRAM entry retain casts
 * because those handlers deliberately expose narrower layout views of the
 * same record.
 * The entries include two that share DisplayEffect_FinishStepWhenIdle. */

void (*gDisplayEffect_apfnSteps[])(MenuRecord *) = {
    (void (*)(MenuRecord *))DisplayEffect_FinishStepWhenIdle,
    (void (*)(MenuRecord *))DisplayEffect_FinishStepWhenIdle,
    (void (*)(MenuRecord *))DisplayEffect_UpdateResourceAcquisition,
    DisplayEffect_UpdatePositionTween,
    DisplayEffect_CrossfadeResources,
    DisplayEffect_UpdatePortraitTransition,
    DisplayEffect_FadeOutRecord,
    DisplayEffect_FadeInRecord,
    Dialog_UpdateMessageTransition,
    Dialog_UpdateOpenTransition,
    Dialog_UpdateCloseTransition,
    Dialog_UpdateSpotlightContraction,
    Dialog_UpdateMessageTransition,
};
