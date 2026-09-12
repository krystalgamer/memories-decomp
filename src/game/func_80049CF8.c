/*
 * The 224-byte mute routine matches with the existing G8 split profile and
 * DATA state-pointer view. Reusing control for the channel/activity tests
 * and packet mask prevents an extra loop-invariant register and stack save.
 * The counter scope preserves register allocation; the state/count snapshot
 * keeps the voice cursor increment before the absolute state reload.
 *
 * Keep the object fields relative to the state base. Forming a separate
 * typed object pointer adds an instruction for the 0x180 array offset.
 * Only the selected SpuVoiceAttr fields are initialized, as its mask requires.
 */
#define D_8009B458_IN_DATA
#include "../types.h"
#include "sound.h"

void func_80049CF8(void)
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
                    packet.volume.left = 0;
                    packet.volume.right = 0;
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
    state->field_07E2 = 4;
    state->flag_0500 = 0;
}
