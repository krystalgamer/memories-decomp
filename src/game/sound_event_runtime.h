#ifndef MEMORIES_DECOMP_SOUND_EVENT_RUNTIME_H
#define MEMORIES_DECOMP_SOUND_EVENT_RUNTIME_H

/* Keep the long return type required by the SDK OpenEvent callback ABI. */
long SD_SequenceTimerCallback(void);

void SD_OpenSequenceTimerEvent(void);
void SD_CloseSequenceTimerEvent(void);
void SD_StopSequenceTimer(void);
void SD_UpdateSecondarySequence(void);

#endif
