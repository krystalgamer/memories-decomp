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
extern u8 gMemCard_szRequestPath[];
extern s32 gMemCard_pRequestBuf;
extern s16 gMemCard_wRequestSize;
extern s16 gMemCard_wRequestOffset;

int MemCard_ReqLoadDirectory(int chan)
{
    if (!MemCard_BeginRequest(chan, 2)) {
        return 0;
    }
    MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
    _card_info(chan);
    while (gMemCard_nIOResult < 0) {
    }
    MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
    _card_clear(gMemCard_bChannel);
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
        sprintf((char *)gMemCard_szRequestPath, (char *)D_80010538, chan, name);
        gMemCard_wRequestOffset = offset;
        gMemCard_pRequestBuf = buf;
        gMemCard_wRequestSize = size;
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
        gMemCard_wRequestOffset = sector;
        gMemCard_pRequestBuf = buf;
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
        sprintf((char *)gMemCard_szRequestPath, (char *)D_80010538, chan, name);
        gMemCard_wRequestOffset = offset;
        gMemCard_pRequestBuf = buf;
        gMemCard_wRequestSize = size;
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
        gMemCard_wRequestOffset = sector;
        gMemCard_pRequestBuf = buf;
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
        sprintf((char *)gMemCard_szRequestPath, (char *)D_80010538, chan, name);
        gMemCard_wRequestSize = blocks;
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
