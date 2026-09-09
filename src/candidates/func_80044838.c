#include "../types.h"
#include "../psyq/libapi.h"

extern u8 D_8009B43E;
extern u8 D_8009B437;
extern u8 D_8009B44F;
extern u8 D_8009B43C;
extern u8 D_8009B436;
extern u16 D_8009B44C;
extern u16 D_8009B434;
extern s32 D_8009B430;
extern s32 D_8009B438;
extern void *D_8009B444;
extern volatile s32 gMemCard_nIOResult;
extern long gMemCard_aIOEventHandles[];
extern long D_800F2AF0[];
extern char D_800F2B00[];

extern void func_80043D48(long *);
extern s32 func_80044608(void);
extern s32 func_80044470(long, char *, void *, s32);

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

    if ((s8)D_8009B43E < 0) {
        return -1;
    }
    if (arg0 != 0) {
        if ((_card_status(D_8009B437 != 0) & 0xE) != 0) {
            return 0;
        }
    } else {
        _card_wait(D_8009B437 != 0);
    }
    switch ((s8)(D_8009B43E - 1)) {
    case 0:
    case 1:
        if (func_80044608() >= 0) {
            goto finish;
        }
        return 0;
    case 10:
    case 11:
        switch (D_8009B44F) {
        case 0:
            r = func_80044608();
            if (r < 0) {
                return 0;
            }
            if (r != 0) {
                gMemCard_nIOResult = 2;
                goto finish;
            }
            D_8009B43C = 0xA;
            D_8009B44F = D_8009B44F + 1;
        case 1:
            func_80043D48(D_800F2AF0);
            _new_card();
            if ((s8)D_8009B43E == 0xB) {
                _card_read(D_8009B437, D_8009B44C, (u8 *)D_8009B430);
            } else {
                _card_write(D_8009B437, D_8009B44C, (u8 *)D_8009B430);
            }
            D_8009B44F = D_8009B44F + 1;
            return 0;
        case 2:
            goto sub_two;
        }
        goto finish;
    case 2:
    case 3:
        switch (D_8009B44F) {
        case 0:
            r = func_80044608();
            if (r < 0) {
                return 0;
            }
            if (r != 0) {
                gMemCard_nIOResult = 2;
                goto finish;
            }
            D_8009B436 = 0x14;
            D_8009B44F = D_8009B44F + 1;
        case 1:
            D_8009B43C = D_8009B43C - 1;
            if ((s8)D_8009B43C < 0) {
                gMemCard_nIOResult = 2;
                goto finish;
            }
            mode = 0x8001;
            if ((s8)D_8009B43E == 4) {
                mode = 0x8002;
            }
            tries = 0xA;
            do {
                fd = open(D_800F2B00, mode);
                tries--;
                if (fd != -1) {
                    goto opened;
                }
            } while (tries >= 0);
            return 0;
opened:
            tries = 0xA;
            do {
                r = lseek(fd, D_8009B44C, 0);
                tries--;
                if (r != -1) {
                    goto seeked;
                }
            } while (tries >= 0);
            goto close_out;
seeked:
            func_80043D48(gMemCard_aIOEventHandles);
            tries = 0xA;
            do {
                if ((s8)D_8009B43E == 4) {
                    r = write(fd, (void *)D_8009B430, D_8009B434);
                } else {
                    r = read(fd, (void *)D_8009B430, D_8009B434);
                }
                tries--;
                if (r == 0) {
                    goto transferred;
                }
            } while (tries >= 0);
            goto close_out;
transferred:
            D_8009B43C = 0x14;
            D_8009B44F = D_8009B44F + 1;
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
        D_8009B44F = 1;
        return 0;
    case 7:
        if (D_8009B44F == 0) {
            goto poll_write;
        }
        if (D_8009B44F == 1) {
            tries = 0xA;
            goto reopen;
        }
        goto finish;
poll_write:
        r = func_80044608();
        if (r < 0) {
            return 0;
        }
        if (r == 0) {
            goto check_size;
        }
        gMemCard_nIOResult = 2;
        goto finish;
check_size:
        if (D_8009B438 + D_8009B434 < 0x10) {
            goto write_dirent;
        }
        gMemCard_nIOResult = 7;
        goto finish;
write_dirent:
        if (func_80044470(D_8009B437, D_800F2B00, D_8009B444, 0) == 0) {
            goto start_reopen;
        }
        gMemCard_nIOResult = 6;
        goto finish;
opened_ok:
        close(fd);
        gMemCard_nIOResult = 0;
        goto finish;
start_reopen:
        D_8009B43C = 0xA;
        D_8009B44F = D_8009B44F + 1;
        tries = 0xA;
reopen:
        do {
            fd = open(D_800F2B00, (D_8009B434 << 16) | 0x200);
            tries--;
            if (fd != -1) {
                goto opened_ok;
            }
        } while (tries >= 0);
        D_8009B43C = D_8009B43C - 1;
        if ((s8)D_8009B43C > 0) {
            return 0;
        }
        gMemCard_nIOResult = 2;
        goto finish;
    }
finish:
    *out_result = gMemCard_nIOResult;
    *out_state = (s8)D_8009B43E;
    D_8009B43E = -1;
    return 1;
}
