#include "../types.h"
#include "fade.h"
#include "file_transfer.h"
#include "game_over.h"
#include "main_modes.h"
#include "main_services.h"
#include "sound.h"

extern u8 D_8009B269;

u8 D_8009B26C;

void Main_RunGameOver(void)
{
    u8 flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C498();
        func_8003C950();
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
