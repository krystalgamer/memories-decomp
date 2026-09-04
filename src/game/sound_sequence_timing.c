#include "../types.h"
#include "sound.h"

void func_8004C5C8(u8 *entry)
{
    switch (D_8009B458->timebase) {
    case 0x30:
        *(s32 *)(entry + 0x1C) = *(s32 *)(entry + 0x1C) * 10;
        *(s32 *)(entry + 0x1C) =
            *(s32 *)(entry + 0x1C) + *(u16 *)(entry + 0x18);
        *(s16 *)(entry + 0x18) = *(u16 *)(entry + 0x1C) & 3;
        *(s32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) >> 2;
        return;
    case 0x60:
        *(s32 *)(entry + 0x1C) = *(s32 *)(entry + 0x1C) * 5;
        *(s32 *)(entry + 0x1C) =
            *(s32 *)(entry + 0x1C) + *(u16 *)(entry + 0x18);
        *(s16 *)(entry + 0x18) = *(u16 *)(entry + 0x1C) & 3;
        *(s32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) >> 2;
        return;
    case 0xC0:
    case 0xF0:
        *(s32 *)(entry + 0x1C) =
            *(s32 *)(entry + 0x1C) + *(u16 *)(entry + 0x18);
        *(s16 *)(entry + 0x18) = *(u16 *)(entry + 0x1C) & 1;
        *(s32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) >> 1;
        return;
    case 0x120:
    case 0x168:
        *(u32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) / 3;
        return;
    case 0x180:
    case 0x1E0:
        *(s32 *)(entry + 0x1C) =
            *(s32 *)(entry + 0x1C) + *(u16 *)(entry + 0x18);
        *(s16 *)(entry + 0x18) = *(u16 *)(entry + 0x1C) & 3;
        *(s32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) >> 2;
        return;
    case 0x300:
    case 0x3C0:
        *(s32 *)(entry + 0x1C) =
            *(s32 *)(entry + 0x1C) + *(u16 *)(entry + 0x18);
        *(s16 *)(entry + 0x18) = *(u16 *)(entry + 0x1C) & 7;
        *(s32 *)(entry + 0x1C) = *(u32 *)(entry + 0x1C) >> 3;
        return;
    default:
        return;
    }
}

extern void func_8004BCE8(void);
extern int func_8004BB34(void *);

int func_8004C77C(void)
{
    register u8 *initial asm("$2") = (u8 *)D_8009B458;
    register int i asm("$17");
    register int offset asm("$18");

    *(int *)(initial + 0x804) = 0;
    initial[0x800] = 0;
    func_8004BCE8();
    {
        register u8 *state asm("$4") = (u8 *)D_8009B458;
        if (*(u16 *)(state + 0x7FA) != 0) {
            int count;

            i = 0;
            offset = 0x518;
            do {
                register u8 *entry asm("$16") = state + offset;
                int value = *(int *)entry;

                *(int *)(entry + 4) = value;
                value = func_8004BB34(entry);
                entry[0x24] = 0;
                entry[0x27] = 0;
                entry[0x29] = 0;
                entry[0x28] = 0;
                {
                    register u8 *flags_state asm("$3") =
                        (u8 *)D_8009B458;
                    *(int *)(entry + 0x1C) = value;
                    *(short *)(entry + 0x18) = 0;
                    if (*(int *)(flags_state + 0x804) != 0) {
                        func_8004C5C8(entry);
                    }
                }
                state = (u8 *)D_8009B458;
                count = *(u16 *)(state + 0x7FA);
                asm volatile("" : "+r"(count));
                i++;
                offset += SD_SEQUENCE_TRACK_RECORD_SIZE;
            } while (i < count);
        }
    }
    return 0;
}
