#include "../types.h"
#include "sound.h"

// Scans 4 entries in g_SDValue's array at +0x404 (stride 2, u16) for ones
// matching the low 16 bits of a0, counting how many also have a nonzero
// SpuGetVoiceEnvelope result written to the stack-local it queries per entry.
extern void func_80077C50(s32 a0, s16 *a1);

s16 func_80047FAC(s32 a0) {
    u16 s1 = 0;
    s32 s0;
    u16 s2 = a0 & 0xFFFF;

    for (s0 = 0; s0 < SD_VOICE_SLOT_COUNT; s0++) {
        s16 local;
        u16 *entry;

        func_80077C50(s0 + 0x14, &local);

        entry = (u16 *)((u8 *)g_SDValue + s0 * 2 + 0x404);
        if (*entry == s2) {
            if (local != 0) {
                s1++;
            }
        }
    }

    return (s16)s1;
}
