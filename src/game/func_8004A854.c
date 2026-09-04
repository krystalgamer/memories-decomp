#include "../types.h"

#define SDSECONDARYSTATE_CUSTOM_EXTERN
#include "sound.h"

extern u8 *D_8009B458;

int func_8004A854(int value)
{
    register int result asm("$9") = -1;
    u8 *state = D_8009B458;
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
            state = D_8009B458;
            count = *(short *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += SD_SECONDARY_OBJECT_SIZE;
        } while (i < count);
    }
    return result;
}
