#include "../types.h"
#include "mem_card.h"

extern u8 D_80010538[];
extern void func_8008F200();
extern void *func_80073AC0();
extern void *func_80073920();

s32 func_80044470(s32 a0, s32 a1, u8 *cursor, s32 *out_count)
{
    u8 work[32];
    s32 retry;
    s32 count;

    func_8008F200(work, D_80010538, a0, a1);
    retry = MEM_CARD_DIRECTORY_RETRIES;
    while ((u8 *)func_80073AC0(work, cursor) != cursor) {
        retry--;
        if (retry < 0)
            return 0;
    }
    retry = MEM_CARD_DIRECTORY_RETRIES;
    count = 1;
    cursor += MEM_CARD_DIRECTORY_ENTRY_SIZE;
    do {
        if ((u8 *)func_80073920(cursor) != cursor) {
            retry--;
            if (retry < 0)
                break;
        } else {
            retry = MEM_CARD_DIRECTORY_RETRIES;
            cursor += MEM_CARD_DIRECTORY_ENTRY_SIZE;
            count++;
        }
    } while (count < MEM_CARD_BLOCK_COUNT);
    if (out_count != (s32 *)0)
        *out_count = count;
    return count;
}
