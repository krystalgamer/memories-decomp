#include "../../types.h"
#include "../../game/fade.h"
#include "../../game/file_transfer.h"
#include "../../game/sound.h"
#include "helpers.h"

s32 func_80168E1C(void)
{
    switch (D_801691FC & 0xF) {
    case 1:
        func_80168A4C();
        break;
    case 2:
        if ((D_801691FC & 0x80) == 0) {
            D_801691FC |= 0x80;
            Fade_StartIn();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            D_801691FC = 1;
        }
        break;
    case 3:
        if ((D_801691FC & 0x80) == 0) {
            D_801691FC |= 0x80;
            func_80168D34(D_80169140);
            SD_BGMFadeOut();
            Fade_StartOut();
        }
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0 &&
            ((D_8009B0F4 & FILE_TRANSFER_REQUEST_BLOCKED_MASK) | D_8009B134) == 0) {
            func_80168D68();
            D_801691FC = 0;
        }
        break;
    }
    func_80168048(D_80169134);
    if (D_801691FC != 0) {
        return -1;
    }
    return D_80169140;
}
