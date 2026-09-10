#include "../types.h"
#include "../psyq/libapi.h"

extern u8 gMemCard_bRequest;
extern u8 gMemCard_bChannel;
extern u8 gMemCard_bRequestStep;
extern u8 gMemCard_bRetries;
extern u8 D_8009B436;
extern u16 gMemCard_wRequestOffset;
extern u16 gMemCard_wRequestSize;
extern s32 gMemCard_pRequestBuf;
extern s32 gMemCard_nFreeBlocks;
extern void *gMemCard_pDirEntries;
extern volatile s32 gMemCard_nIOResult;
extern long gMemCard_aIOEventHandles[];
extern long gMemCard_aHwIOEventHandles[];
extern char gMemCard_szRequestPath[];

extern void MemCard_ClearIOEvents(long *);
extern s32 MemCard_DoLoadDirectory(void);
extern s32 MemCard_FindFiles(long, char *, void *, s32);

/*
 * Current best under gcc_2_8_1_g8_split: 286 instructions against 295,
 * opcode distance 21 and 288 differing positions. The state and sub-state
 * switch structure, fallthroughs, retry loops, signed selector, and card
 * channel predicate reproduce the current shape. Residual: GCC cross-jumps
 * the result-code 2, 6 and 7 store/jump/nop tails that retail keeps separate,
 * removing three stores, six delay-slot/control instructions, and one saved
 * register from the candidate.
 */
s32 func_80044838(s32 arg0, s32 *out_state, s32 *out_result)
{
    s32 r;
    s32 fd;
    s32 tries;
    s32 mode;

    if ((s8)gMemCard_bRequest < 0) {
        return -1;
    }
    if (arg0 != 0) {
        if ((_card_status(gMemCard_bChannel != 0) & 0xE) != 0) {
            return 0;
        }
    } else {
        _card_wait(gMemCard_bChannel != 0);
    }
    switch ((s8)(gMemCard_bRequest - 1)) {
    case 0:
    case 1:
        if (MemCard_DoLoadDirectory() >= 0) {
            goto finish;
        }
        return 0;
    case 10:
    case 11:
        switch (gMemCard_bRequestStep) {
        case 0:
            r = MemCard_DoLoadDirectory();
            if (r < 0) {
                return 0;
            }
            if (r != 0) {
                gMemCard_nIOResult = 2;
                goto finish;
            }
            gMemCard_bRetries = 0xA;
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
        case 1:
            MemCard_ClearIOEvents(gMemCard_aHwIOEventHandles);
            _new_card();
            if ((s8)gMemCard_bRequest == 0xB) {
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
        switch (gMemCard_bRequestStep) {
        case 0:
            r = MemCard_DoLoadDirectory();
            if (r < 0) {
                return 0;
            }
            if (r != 0) {
                gMemCard_nIOResult = 2;
                goto finish;
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
            if ((s8)gMemCard_bRequest == 4) {
                mode = 0x8002;
            }
            tries = 0xA;
            do {
                fd = open(gMemCard_szRequestPath, mode);
                tries--;
                if (fd != -1) {
                    goto opened;
                }
            } while (tries >= 0);
            return 0;
opened:
            tries = 0xA;
            do {
                r = lseek(fd, gMemCard_wRequestOffset, 0);
                tries--;
                if (r != -1) {
                    goto seeked;
                }
            } while (tries >= 0);
            goto close_out;
seeked:
            MemCard_ClearIOEvents(gMemCard_aIOEventHandles);
            tries = 0xA;
            do {
                if ((s8)gMemCard_bRequest == 4) {
                    r = write(fd, (void *)gMemCard_pRequestBuf,
                              gMemCard_wRequestSize);
                } else {
                    r = read(fd, (void *)gMemCard_pRequestBuf,
                             gMemCard_wRequestSize);
                }
                tries--;
                if (r == 0) {
                    goto transferred;
                }
            } while (tries >= 0);
            goto close_out;
transferred:
            gMemCard_bRetries = 0x14;
            gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
close_out:
            close(fd);
            return 0;
        case 2:
            goto sub_two;
        }
        goto finish;
sub_two:
        if (gMemCard_nIOResult == 0) {
            goto finish;
        }
        D_8009B436 = D_8009B436 - 1;
        if ((s8)D_8009B436 < 0) {
            goto finish;
        }
        gMemCard_bRequestStep = 1;
        return 0;
    case 7:
        if (gMemCard_bRequestStep == 0) {
            goto poll_write;
        }
        if (gMemCard_bRequestStep == 1) {
            tries = 0xA;
            goto reopen;
        }
        goto finish;
poll_write:
        r = MemCard_DoLoadDirectory();
        if (r < 0) {
            return 0;
        }
        if (r == 0) {
            goto check_size;
        }
        gMemCard_nIOResult = 2;
        goto finish;
check_size:
        if (gMemCard_nFreeBlocks + gMemCard_wRequestSize < 0x10) {
            goto write_dirent;
        }
        gMemCard_nIOResult = 7;
        goto finish;
write_dirent:
        if (MemCard_FindFiles(gMemCard_bChannel, gMemCard_szRequestPath,
                              gMemCard_pDirEntries, 0) == 0) {
            goto start_reopen;
        }
        gMemCard_nIOResult = 6;
        goto finish;
opened_ok:
        close(fd);
        gMemCard_nIOResult = 0;
        goto finish;
start_reopen:
        gMemCard_bRetries = 0xA;
        gMemCard_bRequestStep = gMemCard_bRequestStep + 1;
        tries = 0xA;
reopen:
        do {
            fd = open(gMemCard_szRequestPath,
                      (gMemCard_wRequestSize << 16) | 0x200);
            tries--;
            if (fd != -1) {
                goto opened_ok;
            }
        } while (tries >= 0);
        gMemCard_bRetries = gMemCard_bRetries - 1;
        if ((s8)gMemCard_bRetries > 0) {
            return 0;
        }
        gMemCard_nIOResult = 2;
        goto finish;
    }
finish:
    *out_result = gMemCard_nIOResult;
    *out_state = (s8)gMemCard_bRequest;
    gMemCard_bRequest = -1;
    return 1;
}
