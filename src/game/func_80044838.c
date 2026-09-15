#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "../types.h"
#include "../psyq/libapi.h"
#include "mem_card.h"
#include "mem_card_directory.h"
#include "io_event_helpers.h"

s32 func_80044838(s32 arg0, s32 *out_state, s32 *out_result)
{
    s32 r;
    s32 fd;
    s32 tries;
    s32 mode;
    s32 resume_step;

    if (gMemCard_bRequest < 0)
        return -1;
    if (arg0 != 0) {
        if ((_card_status(gMemCard_bChannel != 0) & 0xE) != 0)
            return 0;
    } else {
        _card_wait(gMemCard_bChannel != 0);
    }
    switch ((s8)(*(u8 *)&gMemCard_bRequest - 1)) {
    case 0:
    case 1:
        if (MemCard_DoLoadDirectory() >= 0)
            goto finish;
        return 0;
    case 10:
    case 11:
        resume_step = 1;
        switch (gMemCard_bRequestStep) {
        case 0:
            {
                s32 directory_result = MemCard_DoLoadDirectory();
                if (directory_result < 0)
                    return 0;
                if (directory_result != 0) {
                    gMemCard_nIOResult = 2;
                    goto finish;
                }
            }
            gMemCard_bRetries = 0xA;
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
        case 1:
            MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
            _new_card();
            if (gMemCard_bRequest == 0xB) {
                _card_read(gMemCard_bChannel, gMemCard_wRequestOffset,
                           (u8 *)gMemCard_pRequestBuf);
            } else {
                _card_write(gMemCard_bChannel, gMemCard_wRequestOffset,
                            (u8 *)gMemCard_pRequestBuf);
            }
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
            return 0;
        case 2:
            goto sub_two;
        }
        goto finish;
    case 2:
    case 3:
        resume_step = 1;
        switch (gMemCard_bRequestStep) {
        case 0:
            {
                s32 directory_result = MemCard_DoLoadDirectory();
                if (directory_result < 0)
                    return 0;
                if (directory_result != 0) {
                    gMemCard_nIOResult = 2;
                    goto finish;
                }
            }
            D_8009B436 = 0x14;
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
        case 1:
            gMemCard_bRetries = gMemCard_bRetries - 1;
            if ((s8)gMemCard_bRetries < 0) {
                gMemCard_nIOResult = 2;
                goto finish;
            }
            mode = 0x8001;
            if (gMemCard_bRequest == 4)
                mode = 0x8002;
            tries = 0xA;
            do {
                fd = open(gMemCard_szRequestPath, mode);
                tries--;
                if (fd != -1)
                    goto opened;
            } while (tries >= 0);
            return 0;
opened:
            tries = 0xA;
            do {
                r = lseek(fd, gMemCard_wRequestOffset, 0);
                if (r != -1)
                    goto seeked;
            } while (--tries >= 0);
            close(fd);
            return 0;
seeked:
            MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
            tries = 0xA;
            do {
                s32 transfer_result;

                if (gMemCard_bRequest == 4) {
                    transfer_result = write(fd, (void *)gMemCard_pRequestBuf,
                                                          gMemCard_wRequestSize);
                } else {
                    transfer_result = read(fd, (void *)gMemCard_pRequestBuf,
                                           gMemCard_wRequestSize);
                }
                if (transfer_result == 0)
                    goto transferred;
            } while (--tries >= 0);
            close(fd);
            return 0;
transferred:
            gMemCard_bRetries = 0x14;
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
            close(fd);
            return 0;
        case 2:
            goto sub_two;
        }
        goto finish;
sub_two:
        if (gMemCard_nIOResult == 0)
            goto finish;
        D_8009B436 = D_8009B436 - 1;
        if ((s8)D_8009B436 < 0)
            goto finish;
        gMemCard_bRequestStep = resume_step;
        return 0;
    case 7:
        if (gMemCard_bRequestStep == 0)
            goto poll_write;
        if (gMemCard_bRequestStep == 1)
            goto reopen;
        goto finish;
poll_write:
        {
            s32 directory_result = MemCard_DoLoadDirectory();
            if (directory_result < 0)
                return 0;
            if (directory_result == 0)
                goto check_size;
        }
        gMemCard_nIOResult = 2;
        goto finish;
check_size:
        if (gMemCard_nFreeBlocks + gMemCard_wRequestSize < 0x10)
            goto write_dirent;
        gMemCard_nIOResult = 7;
        goto finish;
write_dirent:
        if (MemCard_FindFiles(gMemCard_bChannel, gMemCard_szRequestPath,
                             gMemCard_pDirEntries, 0) == 0)
            goto start_reopen;
        gMemCard_nIOResult = 6;
        goto finish;
opened_ok:
        close(fd);
        gMemCard_nIOResult = 0;
        goto finish;
start_reopen:
        gMemCard_bRetries = 0xA;
        gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
reopen:
        /* Ten retries after the first BIOS open attempt. */
        for (tries = 0xA;;) {
            fd = open(gMemCard_szRequestPath,
                      (gMemCard_wRequestSize << 16) | 0x200);
            if (fd != -1)
                goto opened_ok;
            if (--tries < 0) {
                gMemCard_bRetries = gMemCard_bRetries - 1;
                if ((s8)gMemCard_bRetries > 0)
                    return 0;
                gMemCard_nIOResult = 2;
                goto finish;
            }
        }
    }
finish:
    *out_result = gMemCard_nIOResult;
    *out_state = gMemCard_bRequest;
    gMemCard_bRequest = -1;
    return 1;
}
