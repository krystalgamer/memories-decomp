#include "../types.h"
#include "../psyq/libapi.h"
#include "../psyq/stdio.h"

#include "mem_card.h"

typedef char MemCardDirectoryEntry_size_must_match[
    sizeof(struct DIRENTRY) == MEM_CARD_DIRECTORY_ENTRY_SIZE ? 1 : -1
];

extern u8 D_80010538[];
extern u8 D_800F2B00[];
extern long gMemCard_aIOEventHandles[];
extern long D_800F2AF0[];
extern s32 D_8009B430;
extern s16 D_8009B434;
extern u8 D_8009B437;
extern s16 D_8009B44C;
extern volatile s32 gMemCard_nIOResult;
extern int func_800440B4(int, int);
extern void func_80043D48(long *);

int func_8004413C(int value)
{
    if (!func_800440B4(value, 2)) {
        return 0;
    }
    func_80043D48(gMemCard_aIOEventHandles);
    _card_info(value);
    while (gMemCard_nIOResult < 0) {
    }
    func_80043D48(D_800F2AF0);
    _card_clear(D_8009B437);
    while (gMemCard_nIOResult < 0) {
    }
    func_80043D48(gMemCard_aIOEventHandles);
    _card_load(value);
    while (gMemCard_nIOResult < 0) {
    }
    return 1;
}

int func_800441DC(int value, int data, int global_data, int small, int extra)
{
    int result;

    if (func_800440B4(value, 3)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, value, data);
        D_8009B44C = small;
        D_8009B430 = global_data;
        D_8009B434 = extra;
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_80044278(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 11)) {
        D_8009B44C = small;
        D_8009B430 = data;
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_800442E4(int value, int data, int global_data, int small, int extra)
{
    int result;

    if (func_800440B4(value, 4)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, value, data);
        D_8009B44C = small;
        D_8009B430 = global_data;
        D_8009B434 = extra;
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_80044380(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 12)) {
        D_8009B44C = small;
        D_8009B430 = data;
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_800443EC(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 8)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, value, data);
        D_8009B434 = small;
        func_80043D48(gMemCard_aIOEventHandles);
        _card_info(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

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
