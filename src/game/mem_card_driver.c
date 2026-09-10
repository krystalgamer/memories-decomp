#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "../types.h"
#include "../psyq/libapi.h"
#include "../psyq/stdio.h"
#include "../psyq/strings.h"

#include "mem_card_begin_request.h"
#include "mem_card_directory.h"
#include "mem_card_io_result_callbacks.h"
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
extern char gMemCard_bRequestStep;
extern s32 gMemCard_nFreeBlocks;
extern u8 gMemCard_aDirEntries[];
extern u8 D_8009AF7C[];

void MemCard_ClearIOEvents(long *handles)
{
    TestEvent(handles[0]);
    TestEvent(handles[1]);
    TestEvent(handles[2]);
    TestEvent(handles[3]);
    gMemCard_nIOResult = -1;
}

s32 MemCard_WaitIOEvent(long *handles, s32 once)
{
    do {
        if (TestEvent(handles[0]) == 1)
            return 0;
        if (TestEvent(handles[1]) == 1)
            return 1;
        if (TestEvent(handles[2]) == 1)
            return 2;
        if (TestEvent(handles[3]) == 1)
            return 3;
    } while (once == 0);
    return -1;
}

void MemCard_Init(long val)
{
    InitCARD(val);
    StartCARD();
    ChangeClearPAD(0);
    _bu_init();
}

void MemCard_CloseIOEvents(void)
{
    long *item = gMemCard_aIOEventHandles;
    int count;

    EnterCriticalSection();
    count = 8;
    do {
        CloseEvent(*item++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

void MemCard_InitIOEvents(void)
{
    register long *items;
    register long (*cb0)(void);
    register long (*cb1)(void);
    register long (*cb2)(void);
    int count;
    {
        register long *base = gMemCard_aIOEventHandles;
        gMemCard_bRequest = -1;
        gMemCard_bDirFlags = 0;
        gMemCard_pDirEntries = 0;
        items = gMemCard_aIOEventHandles;
        EnterCriticalSection();
        cb0 = MemCard_SetIOResultCompleteCB;
        base[0] = OpenEvent(SwCARD, EvSpIOE, EvMdINTR, cb0);
        cb1 = MemCard_SetIOResultTimeoutCB;
        items[1] = OpenEvent(SwCARD, EvSpTIMOUT, EvMdINTR, cb1);
    }
    cb2 = MemCard_SetIOResultErrorCB;
    items[2] = OpenEvent(SwCARD, EvSpERROR, EvMdINTR, cb2);
    {
        register long (*cb3)(void) = MemCard_SetIOResultNewCardCB;
        items[3] = OpenEvent(SwCARD, EvSpNEW, EvMdINTR, cb3);
        items[4] = OpenEvent(HwCARD, EvSpIOE, EvMdINTR, cb0);
        items[5] = OpenEvent(HwCARD, EvSpTIMOUT, EvMdINTR, cb1);
        items[6] = OpenEvent(HwCARD, EvSpERROR, EvMdINTR, cb2);
        items[7] = OpenEvent(HwCARD, EvSpNEW, EvMdINTR, cb3);
    }
    count = 8;
    do {
        EnableEvent(*items++);
        count--;
    } while (count != 0);
    ExitCriticalSection();
}

void MemCard_ClearCard(int chan)
{
    int count = 10;

    do {
        MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
        _card_clear(chan);
        while (gMemCard_nIOResult < 0) {
        }
        if (gMemCard_nIOResult != 1)
            break;
        count--;
    } while (count > 0);
}

s32 MemCard_BeginRequest(s32 chan, s32 request)
{
    if (gMemCard_bRequest >= 0)
        return 0;
    gMemCard_bRetries = 10;
    gMemCard_bChannel = chan;
    gMemCard_bRequest = request;
    gMemCard_bRequestStep = 0;
    gMemCard_bLoadStep = 0;
    gMemCard_nIOResult = -1;
    return 1;
}

int MemCard_ReqCardInfo(int chan)
{
    int result;
    if (MemCard_BeginRequest(chan, 1)) {
        MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
        _card_info(chan);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

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

s32 MemCard_CalcFreeBlocks(u8 *entry, s32 count)
{
    s32 i;
    s32 total = 0;

    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        s32 value = *(s32 *)(entry + 24);

        total += value / MEM_CARD_BLOCK_SIZE;
        if (value % MEM_CARD_BLOCK_SIZE) {
            total++;
        }
    }
    return MEM_CARD_BLOCK_COUNT - total;
}

s32 MemCard_FindEntry(u8 *name, u8 *entry, s32 count)
{
    s32 i;

    for (i = 0; i < count; i++, entry += MEM_CARD_DIRECTORY_ENTRY_SIZE) {
        if (strcmp(entry, name) == 0) {
            return i;
        }
    }
    return -1;
}

s32 MemCard_DoLoadDirectory(void) {
    s32 v0;
    s32 one;
    s32 v1;

    v1 = gMemCard_bLoadStep;
    if (v1 == 1) {
        goto state1;
    }
    if (v1 < 2) {
        if (v1 == 0) {
            goto state0;
        }
        goto ret;
    }
    if (v1 == 2) {
        goto state2;
    }
    goto ret;

state0:
    v1 = gMemCard_nIOResult;
    if (v1 == 1) {
        goto state0_info;
    }
    if (v1 < 2) {
        if (v1 != 0) {
            goto ret;
        }
    } else {
        if (v1 == 2) {
            goto ret;
        }
        if (v1 == 3) {
            goto sub_poll_entry;
        }
        goto ret;
    }
    goto state0_zero;

state0_info:
    v0 = gMemCard_bRetries - 1;
    gMemCard_bRetries = (u8)v0;
    if ((s8)v0 == 0) {
        goto ret;
    }
    MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
    _card_info(gMemCard_bChannel);
    return -1;

state0_zero:
    if (gMemCard_bDirFlags & 0x80) {
        if (gMemCard_bRequest != 8) {
            goto ret;
        }
    }
    gMemCard_nIOResult = 3;
sub_poll_entry:
    if (gMemCard_bRequest == 1) {
        goto ret;
    }
    gMemCard_bRetries = 0xA;
    gMemCard_bLoadStep = (u8)(gMemCard_bLoadStep + 1);
sub_retry:
    MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
    _card_clear(gMemCard_bChannel);
    return -1;

state1:
    v0 = gMemCard_nIOResult;
    if (v0 == 0) {
        goto state1_zero;
    }
    v1 = gMemCard_nIOResult;
    if (v1 != 2) {
        goto ret;
    }
    v0 = gMemCard_bRetries - 1;
    gMemCard_bRetries = (u8)v0;
    if ((s8)v0 > 0) {
        goto sub_retry;
    }
    goto ret;

state1_zero:
    gMemCard_bRetries = 0xA;
    gMemCard_bLoadStep = 2;
load_retry:
    MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
    _card_load(gMemCard_bChannel);
    return -1;

state2:
    v0 = gMemCard_nIOResult;
    if (v0 == 2) {
        v0 = gMemCard_bRetries - 1;
        gMemCard_bRetries = (u8)v0;
        if ((s8)v0 > 0) {
            goto load_retry;
        }
    }

    gMemCard_bDirFlags |= 0x80;
    v1 = gMemCard_nIOResult;
    if (v1 != 0) {
        goto after_load;
    }
    gMemCard_pDirEntries = gMemCard_aDirEntries;
    MemCard_FindFiles(gMemCard_bChannel, (const char *)D_8009AF7C,
                      (struct DIRENTRY *)gMemCard_aDirEntries,
                      &gMemCard_nDirEntries);
    gMemCard_nFreeBlocks =
        MemCard_CalcFreeBlocks(gMemCard_pDirEntries, gMemCard_nDirEntries);

after_load:
    if (gMemCard_nIOResult == 3) {
        gMemCard_nIOResult = 4;
    }
ret:
    return gMemCard_nIOResult;
}
