/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 5 variables to hard registers and 2 inline asm statements, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_secondary_object_selection.c.
 */
#include "../types.h"
#include "../game/sound.h"
#include "../game/sound_secondary_object_selection.h"

int func_8004A854(int value)
{
    register int result asm("$9") = -1;
    u8 *state = (u8 *)D_8009B458;
    register short best asm("$8");
    register int i asm("$5");
    asm volatile("li %0,0xffff" : "=r"(best));
    if (*(short *)(state + 0x510) > 0) {
        register int offset asm("$7");
        int count;
        i = 0;
        value = (u8)value;
        offset = i;
        do {
            u8 *entry = state + offset;
            register u16 candidate asm("$6") =
                *(u16 *)(entry + 0x19E);
            if ((u16)best >= candidate && entry[0x183] == value &&
                entry[0x18D] != 0) {
                best = candidate;
                result = i;
            }
            state = (u8 *)D_8009B458;
            count = *(short *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    return result;
}

