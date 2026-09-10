/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 6 variables to hard registers and 3 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_secondary_playback.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_spatialization.h"
#define SD_START_SEQUENCE_TRACKS_PLAYBACK_CALLS
#include "../game/sound_sequence_timing.h"

void func_80049CF8(void)
{
    SpuVoiceAttr packet;
    register u8 *initial asm("$2") = (u8 *)D_8009B458;
    register u8 *state asm("$3");
    register s32 i asm("$16");
    register s32 offset asm("$17");
    register s32 *voice_bits asm("$18");

    initial[0x500] = 1;
    state = (u8 *)D_8009B458;
    if (*(s16 *)(state + 0x510) > 0) {
        s32 count;

        i = 0;
        asm volatile(
            "lui $2,%%hi(D_80011434)\n\t"
            "addiu %0,$2,%%lo(D_80011434)"
            : "=r"(voice_bits)
            :
            : "$2"
        );
        offset = i;
        do {
            u8 *entry = state + offset;

            if ((entry[0x183] >> 4) == 0 && entry[0x18D] != 0) {
                register s32 mask asm("$2") = SPU_VOICE_VOLL | SPU_VOICE_VOLR;

                packet.mask = mask;
                packet.voice = *voice_bits;
                packet.volume.left = 0;
                packet.volume.right = 0;
                SpuSetVoiceAttr(&packet);
            }
            voice_bits++;
            asm volatile("" : "+r"(voice_bits));
            state = (u8 *)D_8009B458;
            count = *(s16 *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    state = (u8 *)D_8009B458;
    *(s16 *)(state + 0x7E2) = 4;
    state[0x500] = 0;
}

