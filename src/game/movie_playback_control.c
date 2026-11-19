#include "../types.h"
#include "main.h"
#include "movie_pipeline.h"
#include "file_transfer.h"
#include "input.h"
#include "movie_playback_control.h"
#include "sound_output_state.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
u8 gMovie_bPlaybackState;

void Movie_Play(s32 index)
{
    File_WaitForTransfers();
    SD_SetCdVolumeImmediate(0xFF);
    gMovie_bPlaybackState = MOVIE_PLAYBACK_STATE_PLAYING;
    gFile_dwTransferState |= 0x2000000;
    Movie_OpenIndexedStream(index, 1, -1, 1, 0);
}

void Movie_UpdatePlayback(void)
{
    if (Movie_StepPlayback() == 0) {
        u8 value = gMovie_bPlaybackState;

        if ((value & MOVIE_PLAYBACK_STATE_STOP_REQUESTED) ||
            (gInput_awPadPressed[0] & PAD_BUTTON_START)) {
            gMovie_bPlaybackState = value | MOVIE_PLAYBACK_STATE_STOP_REQUESTED;
            gGraphics_bClearRed = 1;
            gGraphics_bClearGreen = 1;
            gGraphics_bClearBlue = 1;
            Movie_StartFadeOut();
        }
    } else {
        gFile_dwTransferState = gFile_dwTransferState & 0xFDFFFFFF;
        gMovie_bPlaybackState &= ~MOVIE_PLAYBACK_STATE_PLAYING;
    }
}

void Movie_ResetPlaybackState(void)
{
    gMovie_bPlaybackState = 0;
}
