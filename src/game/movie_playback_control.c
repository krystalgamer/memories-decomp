#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#define D_8009B142_IS_AGGREGATE
#include "graphics_frame.h"
#include "file_cd_helpers.h"
#include "file_transfer.h"
#include "input.h"
#include "mdec_sync.h"
#include "movie_playback_control.h"
#include "movie_stream_requests.h"
#include "sound_mix.h"

void Movie_Play(s32 index)
{
    File_WaitForTransfers();
    SD_SetCdVolumeImmediate(0xFF);
    gMovie_bPlaybackState = 0x80;
    D_8009B0F4_abs |= 0x2000000;
    Movie_OpenIndexedStream(index, 1, -1, 1, 0);
}

void Movie_UpdatePlayback(void)
{
    if (Movie_StepPlayback() == 0) {
        u8 value = gMovie_bPlaybackState;

        if ((value & 0x40) ||
            (gInput_wPad1Pressed[0] & PAD_BUTTON_START)) {
            gMovie_bPlaybackState = value | 0x40;
            gGraphics_bClearRed[0] = 1;
            gGraphics_bClearGreen[0] = 1;
            gGraphics_bClearBlue[0] = 1;
            Movie_StartFadeOut();
        }
    } else {
        D_8009B0F4_abs = D_8009B0F4_abs & 0xFDFFFFFF;
        gMovie_bPlaybackState &= 0x7F;
    }
}

void Movie_ResetPlaybackState(void)
{
    gMovie_bPlaybackState = 0;
}
