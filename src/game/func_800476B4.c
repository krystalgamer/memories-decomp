#include "../types.h"

typedef struct {
    u8 pad0000[6];
    u16 field_0006;
} SoundPendingEntry;

typedef struct {
    u8 pad0000[0x43C];
    u16 *field_043C;
    u16 field_0440;
    u8 pad0442[2];
    SoundPendingEntry *field_0444;
} SoundPendingState;

extern SoundPendingState *g_SDValue;

void func_800476B4(void *arg0, u32 arg1)
{
    register s32 i asm("$7");
    u8 *base = arg0;

    i = 0;
    if (*(s32 *)base > 0) {
        u8 *payload;
        register u8 *id_cursor asm("$6");
        u32 rate;
        register s32 sentinel asm("$11");

        sentinel = 0xFFFF;
        rate = arg1 >> 4;
        payload = base;
        id_cursor = base;

        do {
            u16 id = *(u16 *)(id_cursor + 8);

            if (id != sentinel) {
                register SoundPendingState *state asm("$5") = g_SDValue;

                state->field_043C[id] = state->field_0440;

                {
                    u16 slot = state->field_0440;
                    SoundPendingEntry *entries = state->field_0444;
                    register SoundPendingEntry *dst asm("$2") =
                        (SoundPendingEntry *)(slot * 8 + (s32)entries);
                    register SoundPendingEntry *updated asm("$3");
                    register SoundPendingState *state2 asm("$4") = state;

                    __builtin_memcpy(dst, payload + 0x1A0, 8);
                    updated = (SoundPendingEntry *)
                        (state2->field_0440 * 8 +
                         (s32)state2->field_0444);
                    updated->field_0006 += rate;
                    state2->field_0440++;
                }
            }

            payload += 8;
            id_cursor += 2;
            i++;
        } while (i < *(s32 *)base);
    }
}
