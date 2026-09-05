#include "../types.h"

void func_8003201C(u8 *state)
{
    state[0x5AC4] = 0;

    {
        register s32 id asm("a2") = 1;
        u8 *count = state + id;

        for (; id < 0x2D3; id++, count++) {
            s32 record_index;
            u8 *record;
            register u8 *output asm("a3");

            count[0x5AC4] = 0;
            record_index = 0;
            output = count;
            record = state + 0x2D54;

            for (; record_index < 40; record_index++) {
                if (record[9] != 0 && *(s16 *)record == id) {
                    output[0x5AC4]++;
                }
                record += 0x10;
            }
        }
    }

    {
        register u8 *record asm("v1") = state + 0x2D50;
        register s32 leading asm("a1") = 0;
        register s32 record_index asm("a2");

        for (record_index = 0; record_index < 40; record_index++) {
            if (record[0xD] == 0) {
                break;
            }
            leading++;
            record += 0x10;
        }

        *(s32 *)(state + 0x5AA0) = leading;
    }
}
