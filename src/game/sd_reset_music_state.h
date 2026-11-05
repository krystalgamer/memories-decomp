#ifndef MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H
#define MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H

/* Retires the driver's music resources: stops and closes the sequence when
   s_wSD_SEQ_ACCESS is active, releases a successful s_wSD_MUSIC_VAB_ID, then
   clears the pending pair at s_wSD_SEQ_FADE_LEVEL/s_wSD_SEQ_FADE_STEP. */
void SD_ResetMusicState(void);

#endif
