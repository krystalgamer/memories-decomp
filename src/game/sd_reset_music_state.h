#ifndef MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H
#define MEMORIES_DECOMP_SD_RESET_MUSIC_STATE_H

/* Retires the driver's music resources: stops and closes the sequence when
   sequence_open_result is active, releases a successful music_vab_open_result, then
   clears the pending pair at sequence_fade_level/sequence_fade_step. */
void SD_ResetMusicState(void);

#endif
