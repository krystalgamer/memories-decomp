#ifndef MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H
#define MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H

/* Retires the driver's music resources: stops and closes the active sequence
   access number at field_157E, releases the VAB id/state at field_157A, then
   clears the pending pair at sequence_fade_level/sequence_fade_step. */
void SD_ResetMusicState(void);

#endif
