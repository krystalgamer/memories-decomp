#include "../types.h"
#include "fade.h"
#include "func_8003C2B4.h"
#include "main_services.h"
#include "options.h"
#include "sound.h"

u8 D_8009B26C;

void Main_RunOptionsMenu(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C2B4();
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
