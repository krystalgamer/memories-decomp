#define MAIN_MODE_STATE_NEXT_AS_SCALAR
#define MAIN_MODE_STATE_ACTIVE_AS_SCALAR
#include "../types.h"
#include "fade.h"
#include "file_transfer.h"
#include "game_over.h"
#include "main_modes.h"
#include "main_services.h"
#include "options.h"
#include "sound.h"
#include "../unmatched.h"
#include "main_mode_state.h"

u8 D_8009B26C;

void Main_RunOptionsMenu(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestOptionsPackage();
        Options_Init();
        Fade_WaitIn();
    }
    if (Options_Update() == 0) {
        u8 value;

        SD_BGMFadeOut();
        value = D_8009B269;
        D_8009B26C = value;
    }
}

void Main_RunGameOver(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestGameOverPackage();
        GameOver_Init();
    }

    if (func_8003CA5C() == 0) {
        u8 mode;

        SD_BGMFadeOut();
        Fade_WaitOut();
        mode = D_8009B269;
        D_8009B26C = mode;
        if (mode != 0) {
            D_8009B268 = 1;
            D_8009B26D = 0;
            D_8009B26C = 8;
            longjmp(D_800E9DC0, 1);
        }
    }
}

/* gMain_apfnModeRunner entry 13: the developer-mode slot's runner is empty in
   the retail build. */
void Main_RunUnusedDeveloperMode(void)
{
}
