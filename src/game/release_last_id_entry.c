#include "../types.h"

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
