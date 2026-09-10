#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "../types.h"
#include "mem_card_directory.h"
#include "mem_card.h"
#include "../psyq/libapi.h"
#include "io_event_helpers.h"

extern s32 gMemCard_nFreeBlocks;

extern u8 gMemCard_aDirEntries[];
extern u8 D_8009AF7C[];

/* The definition in mem_card_requests.c returns s32 and takes four
   arguments; the empty parentheses here only disabled checking, and every
   call already writes all four. The result is discarded at the one call
   site, so naming the real return type changes nothing. */
s32 MemCard_FindFiles(s32 chan, const char *pattern, struct DIRENTRY *cursor,
                      s32 *out_count);
s32 MemCard_DoLoadDirectory(void) {
    s32 v0;
    register s32 one asm("a0");
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
