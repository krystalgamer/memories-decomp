#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "ai.h"
#include "scene_script.h"

/* Zero-initialized storage this unit owns at 0x800EAE88-0x800EAED8, in
 * address order (#2602). Each common is aligned by its size, so the slot
 * table starts at +0x10; only three slots fit before
 * gDebug_szEditorCaretLine, matching SceneScript_UpdateSlots' sweep. */
AiSelection gAi_Selection;
SceneScriptSlot gSceneScript_Slots[3];

#include "duel_effect.h"
#include "fade.h"
#include "script_command_runner.h"
#include "sound.h"
#include "campaign_scene_package.h"
#include "../unmatched.h"
#include "main_mode_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMain_bActiveModeState;

void Main_RunCampaign(void) {
    u8 flags = gMain_bActiveModeState;

    if ((flags & 0x40) == 0) {
        gMain_bActiveModeState = flags | 0x40;
        Campaign_LoadScenePackage(gCampaignSceneIndex);
        gDuel_bEffectState = 0;
        return;
    }

    SceneScript_UpdateSlots(gSceneScript_Slots);
    Script_RunTick();
    if (gMain_bActiveModeState & 0x40)
        return;
    SD_BGMFadeOut();
    Fade_WaitOut();
}
