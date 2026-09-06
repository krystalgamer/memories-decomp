#include "../types.h"
#include "card_constants.h"

extern void func_80032C48(void *records);

void func_80031F7C(u8 *state, s32 id)
{
    s32 count = (state + id)[0x5D97];

    if (count != 0) {
        count--;
        if (count == 0) {
            u8 *record = state + 4;

            while (1) {
                if (*(s16 *)(record + 4) == id) {
                    break;
                }
                record += 0x10;
            }

            record[0xD] = 0;
            if ((state + id)[0x5D97] != 0) {
                record[0xD] = 0x80;
            }
            func_80032C48(state + 4);
        }
        (state + id)[0x5D97] = count;
        *(s32 *)(state + 0x5A9C) -= 1;
    }
}

void func_8003201C(u8 *state)
{
    state[0x5AC4] = 0;

    {
        register s32 id asm("a2") = 1;
        u8 *count = state + id;

        for (; id < CARD_COUNT + 1; id++, count++) {
            s32 record_index;
            u8 *record;
            register u8 *output asm("a3");

            count[0x5AC4] = 0;
            record_index = 0;
            output = count;
            record = state + 0x2D54;

            for (; record_index < DECK_SIZE; record_index++) {
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

        for (record_index = 0; record_index < DECK_SIZE; record_index++) {
            if (record[0xD] == 0) {
                break;
            }
            leading++;
            record += 0x10;
        }

        *(s32 *)(state + 0x5AA0) = leading;
    }
}
