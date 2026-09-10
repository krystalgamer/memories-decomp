#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "../types.h"
#include "../psyq/libapi.h"
#include "../psyq/stdio.h"

#include "mem_card_begin_request.h"
#include "io_event_helpers.h"
#include "mem_card.h"

typedef char MemCardDirectoryEntry_size_must_match[
    sizeof(struct DIRENTRY) == MEM_CARD_DIRECTORY_ENTRY_SIZE ? 1 : -1
];

extern u8 D_80010538[];
extern u8 D_800F2B00[];
extern s32 D_8009B430;
extern s16 D_8009B434;
extern s16 D_8009B44C;

int MemCard_ReqLoadDirectory(int chan)
{
    if (!MemCard_BeginRequest(chan, 2)) {
        return 0;
    }
    MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
    _card_info(chan);
    while (gMemCard_nIOResult < 0) {
    }
    MemCard_ClearIOEvents(D_800F2AF0);
    _card_clear(D_8009B437);
    while (gMemCard_nIOResult < 0) {
    }
    MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
    _card_load(chan);
    while (gMemCard_nIOResult < 0) {
    }
    return 1;
}

int MemCard_ReqReadFile(int chan, int name, int buf, int offset, int size)
{
    int result;

    if (MemCard_BeginRequest(chan, 3)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, chan, name);
        D_8009B44C = offset;
        D_8009B430 = buf;
        D_8009B434 = size;
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int MemCard_ReqReadSector(int chan, int buf, int sector)
{
    int result;

    if (MemCard_BeginRequest(chan, 11)) {
        D_8009B44C = sector;
        D_8009B430 = buf;
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int MemCard_ReqWriteFile(int chan, int name, int buf, int offset, int size)
{
    int result;

    if (MemCard_BeginRequest(chan, 4)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, chan, name);
        D_8009B44C = offset;
        D_8009B430 = buf;
        D_8009B434 = size;
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int MemCard_ReqWriteSector(int chan, int buf, int sector)
{
    int result;

    if (MemCard_BeginRequest(chan, 12)) {
        D_8009B44C = sector;
        D_8009B430 = buf;
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int MemCard_ReqCreateFile(int chan, int name, int blocks)
{
    int result;

    if (MemCard_BeginRequest(chan, 8)) {
        sprintf((char *)D_800F2B00, (char *)D_80010538, chan, name);
        D_8009B434 = blocks;
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

s32 MemCard_FindFiles(s32 chan, const char *pattern, struct DIRENTRY *cursor,
                      s32 *out_count)
{
    char work[32];
    s32 retry;
    s32 count;

    sprintf(work, (char *)D_80010538, chan, pattern);
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
