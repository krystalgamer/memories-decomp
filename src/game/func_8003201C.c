#include "../types.h"
#include "build_deck_card_counts.h"
#include "card_constants.h"

void func_8003201C(u8 *state)
{
    s32 index;

    state[0x5AC4] = 0;
    {
        u8 *count;

        index = CARD_ID_FIRST;
        count = state + index;
        for (; index < CARD_ID_END; index++, count++) {
            s32 record_index;
            u8 *record;
            u8 *output;

            count[0x5AC4] = 0;
            record_index = 0;
            output = count;
            record = state + 0x2D54;
            for (; record_index < DECK_SIZE; record_index++) {
                if (record[9] != 0 && *(s16 *)record == index)
                    output[0x5AC4]++;
                record += 0x10;
            }
        }
    }
    {
        u8 *record;
        s32 leading;

        record = state + 0x2D50;
        leading = 0;
        for (index = 0; index < DECK_SIZE; index++) {
            if (record[0xD] == 0)
                break;
            leading++;
            record += 0x10;
        }
        *(s32 *)(state + 0x5AA0) = leading;
    }
}
