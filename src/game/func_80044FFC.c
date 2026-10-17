#include "../types.h"
#include "sound.h"

/* Queues CD fade command 41: field_0008 is the signed per-update step and
   field_0002 the target volume, loaded into field_0512/field_0049 when
   SD_UpdateRuntime dequeues it. field_0001 becomes field_007E; while it is
   nonzero, SD_StepActiveCommand keeps the command active until the fade
   ends. No caller is present in the retail executable or its overlays. */
s32 SD_EnqueueCdFade(s32 a0, s32 a1, s32 a2) {
    SDCommand s;
    s.command = 41;
    s.field_0008 = (s16) a0;
    s.field_0002 = (s16) (u8) a1;
    s.field_0001 = (u8) a2;
    return SD_EnqueueCommand(&s);
}
