#define D_8009B254_IN_DATA
#include "../types.h"
#include "scene_script.h"
#include "duel_effect.h"
#include "fade.h"
#include "script_run_tick.h"
#include "sound.h"

extern u8 D_8009B26C;
extern u8 gCampaignSceneIndex __attribute__((section(".data")));
extern void func_8002FD10(s32);
extern void func_8002FFD4(void *);

void Main_RunCampaign(void) {
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8002FD10(gCampaignSceneIndex);
        D_8009B254 = 0;
        return;
    }

    func_8002FFD4((u8 *)D_800EAE98);
    Script_RunTick();
    if (D_8009B26C & 0x40)
        return;
    SD_BGMFadeOut();
    Fade_WaitOut();
}
