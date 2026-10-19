#ifndef MEMORIES_DECOMP_SD_RESET_SECONDARY_PLAYBACK_THUNK_H
#define MEMORIES_DECOMP_SD_RESET_SECONDARY_PLAYBACK_THUNK_H

/* A one-call wrapper around SD_ResetSecondaryPlayback. Its three callers are
   the secondary-driver lifecycle entry points: SD_InitSecondaryWorkArea after
   SpuInit, SD_ResetSecondaryRuntime, and SD_Term before SpuQuit. Each one
   calls SD_ResetSecondaryState afterwards. */
void SD_ResetSecondaryPlaybackThunk(void);

#endif
