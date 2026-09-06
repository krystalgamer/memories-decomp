#include "../types.h"
#include "../psyq/libapi.h"
#include "../psyq/stdio.h"
#include "mem_card.h"

typedef char MemCardDirectoryEntry_size_must_match[
    sizeof(struct DIRENTRY) == MEM_CARD_DIRECTORY_ENTRY_SIZE ? 1 : -1
];

extern u8 D_80010538[];

s32 func_80044470(s32 a0, s32 a1, struct DIRENTRY *cursor, s32 *out_count)
{
    char work[32];
    s32 retry;
    s32 count;

    sprintf(work, (char *)D_80010538, a0, a1);
    retry = MEM_CARD_DIRECTORY_RETRIES;
    while (firstfile(work, cursor) != cursor) {
        retry--;
        if (retry < 0)
            return 0;
    }
    retry = MEM_CARD_DIRECTORY_RETRIES;
    count = 1;
    cursor++;
    do {
        if (nextfile(cursor) != cursor) {
            retry--;
            if (retry < 0)
                break;
        } else {
            retry = MEM_CARD_DIRECTORY_RETRIES;
            cursor++;
            count++;
        }
    } while (count < MEM_CARD_BLOCK_COUNT);
    if (out_count != (s32 *)0)
        *out_count = count;
    return count;
}
