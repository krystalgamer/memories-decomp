#define GDUEL_BEFFECTSTATE_IN_DATA
#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"

/* Zero-initialized storage this unit owns at 0x800EAE88-0x800EAED8, defined in
 * address order ahead of the headers that declare it (#2602). */
u8 D_800EAE88[0x50];

#include "scene_script.h"
#include "duel_effect.h"
#include "fade.h"
#include "script_run_tick.h"
#include "sound.h"
#include "campaign_scene_package.h"
#include "../unmatched.h"
#include "main_mode_state.h"

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
