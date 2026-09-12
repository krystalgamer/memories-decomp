/*
 * The 240-byte restoration routine matches with the existing G8 split
 * profile and DATA state-pointer view. Reusing control for the two tests
 * and packet mask prevents an extra loop-invariant register and stack save.
 * The counter scope preserves allocation; the state/count snapshot keeps
 * the bound load before the index increment.
 *
 * Object fields stay relative to the state base to preserve retail offsets.
 * The SDK mask selects only the initialized voice and cached volume fields.
 */
#define D_8009B458_IN_DATA
#include "../types.h"
#include "sound.h"

void func_80049DD8(void)
{
    SpuVoiceAttr packet;
    SDSecondaryState *initial = D_8009B458;
    SDSecondaryState *state;
    s32 i;
    s32 offset;
    s32 *voice_bits;

    initial->flag_0500 = 1;
    state = D_8009B458;
    if (state->object_count > 0) {
        s32 count;

        do {
            i = 0;
        } while (0);
        voice_bits = D_80011434;
        offset = i;
        do {
            u8 *entry = (u8 *)state + offset;
            u32 control = entry[
                (u32)&((SDSecondaryState *)0)->objects[0].channel_index
            ] >> 4;

            if (control == 0) {
                control = entry[
                    (u32)&((SDSecondaryState *)0)->objects[0].field_000D
                ];
                if (control != 0) {
                    control = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
                    packet.mask = control;
                    packet.voice = *voice_bits;
                    packet.volume.left = *(u16 *)(entry +
                        (u32)&((SDSecondaryState *)0)->objects[0].level_left);
                    packet.volume.right = *(u16 *)(entry +
                        (u32)&((SDSecondaryState *)0)->objects[0].level_right);
                    SpuSetVoiceAttr(&packet);
                }
            }
            voice_bits++;
            do {
                state = D_8009B458;
                count = state->object_count;
            } while (0);
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    state = D_8009B458;
    state->field_07E2 = 1;
    state->flag_0500 = 0;
}
