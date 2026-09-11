/*
 * Pumps the sound driver's command state at g_SDValue+0x7C. Current best
 * under gcc_2_8_1_g8_split: 437/437 instructions, opcode multiset distance 4,
 * and 342 differing positions, with no hard register assignments.
 *
 * The raw 82-entry switch keeps its eight distinct targets in retail layout
 * order. Absolute pointer-global declarations, switch-shaped +0x7D
 * sub-dispatches, a zero-based case 0, per-block pointer caching, and the
 * three ordered clear exits recover the exact instruction count.
 *
 * Residual: the stored source merges case 33 and case 36 through one call,
 * leaving one jal/sw missing and one lui/lw surplus, plus two smaller
 * scheduling clusters. The eventual matching promotion will also need the
 * documented jtbl_80010578 rodata split; candidate storage does not.
 */
#define G_SDVALUE_IN_DATA
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#define SOUND_TRANSFER_REQUEST_IN_DATA
#include "../types.h"
#include "../game/sound_transfer_lifecycle.h"
#include "../game/file_transfer.h"
#include "../game/sound.h"

typedef struct {
    u8 pad_00[0x40];
    u16 flags;
    u8 pad_42[7];
    u8 b49;
    u8 pad_4A[4];
    u16 h4E;
    s32 w50;
    u16 h54;
    u8 pad_56[2];
    u8 *p58;
    s32 w5C;
    s32 w60;
    u8 pad_64[4];
    s32 w68;
    s32 w6C;
    s32 w70;
    s32 w74;
    u8 pad_78[4];
    u8 b7C;
    u8 b7D;
    u8 b7E;
    u8 pad_7F[0x4CC - 0x7F];
    s32 w4CC;
    u8 pad_4D0[0x510 - 0x4D0];
    s16 h510;
    s16 h512;
    u8 pad_514[0x528 - 0x514];
    u32 w528;
    u32 w52C;
    u8 b530;
    u8 b531;
    u8 b532;
    u8 b533;
    u16 h534;
    u8 pad_536[0x1564 - 0x536];
    u8 *p1564;
    u8 pad_1568[0x157A - 0x1568];
    s16 h157A;
    u16 h157C;
    s16 h157E;
    u8 pad_1580[0x1586 - 0x1580];
    s16 h1586;
    u16 h1588;
    u8 b158A;
    u8 pad_158B[0x1618 - 0x158B];
    u8 b1618;
    u8 b1619[0x10];
    u8 b1629[0x10];
    u8 b1639[0x10];
} SD;

typedef struct {
    u32 a;
    u32 b;
} SoundCommandPair;

typedef struct {
    u16 h0;
    u16 count;
    u8 pad_04[0xC];
    u16 idx[1];
} List;

#define SD_STATE ((SD *)g_SDValue)

extern void func_800476B4(u8 *, s32);
extern u8 func_80045484(void);
extern void SD_ArmBusyCallback(void);
extern s16 func_80049A64(u8 *, s16);
extern void func_80049AF4(s32, s32);

void func_80045514(void)
{
    switch (SD_STATE->b7C) {
    case 81:
        func_800476B4(SD_STATE->p58, SD_STATE->w50);
        goto clear_7d_7c;

    case 32:
        switch (SD_STATE->b7D) {
        case 0:
            if (func_80014C40(0, 0) != 0) {
                return;
            }
            if (SD_STATE->w4CC != 0) {
                return;
            }
            {
                SD *sd = SD_STATE;
                FileRequestSlot *b = D_8009B460;

                b->field_10 = sd->w6C;
                b->field_0C = sd->w68;
                b->field_04 = sd->w60;
                b->field_18 = sd->w74;
                b->field_1F = 0;
                b->field_1C = 0;
                b->field_14 = sd->w70;
                D_8009B460->field_1E = 0;
            }
            D_8009B460->field_00 = SD_STATE->w5C;
            if (D_8009B460->field_18 == 0 && D_8009B460->field_14 == 0) {
                SD_STATE->b7D = 0;
                goto clear_7c;
            }
            switch (SD_STATE->w5C & 0xF0) {
            case 0x10:
                D_8009B460->field_00 = 4;
                func_80014C40(D_8009B460, SD_STATE->b1629);
                break;
            case 0x20:
                D_8009B460->field_00 = 5;
                func_80014C40(D_8009B460, SD_STATE->b1619);
                break;
            case 0x40:
                D_8009B460->field_00 = 6;
                func_80014C40(D_8009B460, SD_STATE->b1639);
                break;
            }
            SD_ArmBusyCallback();
            SD_STATE->b7D = SD_STATE->b7D + 1;
            return;
        case 1:
            break;
        default:
            return;
        }
        if (SD_STATE->b1618 != 0) {
            return;
        }
        SD_STATE->b7D = 0;
        SD_STATE->b7C = 0;
        if ((func_80045484() & 0xFF) != 0) {
            return;
        }
        SD_STATE->flags = SD_STATE->flags & 0xFFFC;
        return;

    case 17:
        switch (SD_STATE->b7D) {
        case 0:
            if (SD_STATE->h510 == 0) {
                SD_STATE->b7D = 1;
                return;
            }
            if (SD_STATE->h512 < 0) {
                return;
            }
            SD_STATE->h512 = -0x20;
            SD_STATE->b49 = 0;
            return;
        case 1:
            break;
        default:
            return;
        }
        SD_STATE->h534 = 0xFFFF;
        if ((func_80045484() & 0xFF) != 0) {
            goto clear_7d_7c;
        }
        SD_STATE->b7D = 0;
        SD_STATE->flags = SD_STATE->flags & 0xFFF8;
        SD_STATE->b7C = 0;
        return;

    case 33:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e = (SoundCommandPair *)(SD_STATE->p58 + SD_STATE->h4E * 8);

            D_8009B460->field_10 = 0;
            D_8009B460->field_18 = 0;
            D_8009B460->field_0C = 0;
            D_8009B460->field_14 = 0;
            D_8009B460->field_1C = 0;
            D_8009B460->field_1F = SD_STATE->b530;
            D_8009B460->field_1E = SD_STATE->b531;
            D_8009B460->field_00 = 6;
            D_8009B460->field_04 = (e->a & 0xFFFFFF) + SD_STATE->w50;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 36:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e = (SoundCommandPair *)(SD_STATE->p58 + SD_STATE->h4E * 8);
            u32 a = e->a;
            u32 b = e->b;

            SD_STATE->w528 = a & 0xFFFFFF;
            SD_STATE->b531 = (a & 0x1F000000) >> 24;
            SD_STATE->w52C = b & 0xFFFFFF;
            SD_STATE->b530 = (b & 0x1F000000) >> 24;
            SD_STATE->b532 = b >> 31;
            SD_STATE->b533 = a >> 29;
            SD_STATE->w528 = SD_STATE->w528 + SD_STATE->w50;
            SD_STATE->w52C = SD_STATE->w52C + SD_STATE->w50;
            D_8009B460->field_10 = 0;
            D_8009B460->field_18 = 0;
            D_8009B460->field_0C = 0;
            D_8009B460->field_14 = 0;
            D_8009B460->field_1C = (u16)SD_STATE->w52C - (u16)SD_STATE->w528 + 0x10;
            D_8009B460->field_1F = SD_STATE->b530;
            D_8009B460->field_1E = SD_STATE->b531;
            D_8009B460->field_00 = 6;
            D_8009B460->field_04 = SD_STATE->w528;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 41:
    case 42:
        if (SD_STATE->b7E != 0 && SD_STATE->h512 != 0) {
            return;
        }
        SD_STATE->b7C = 0;
        goto clear_7d;

    case 72:
        if (SD_STATE->h157A != 0) {
            u8 *list = SD_STATE->p1564;

            SD_STATE->h157A = func_800496C4(list + 0x50, 0,
                                             *(s32 *)(list + 0xC));
            if ((s16)SD_STATE->h157A != 0) {
                SD_STATE->b7C = 0;
                return;
            }
        }
        if (SD_STATE->h157E == 0) {
            if ((SD_STATE->flags & 0x80) != 0) {
                func_80049C40(0);
                func_80049F10(0, 0);
                SD_STATE->h1586 = 0;
                SD_STATE->h1588 = 0;
                SD_STATE->flags = SD_STATE->flags & 0xFF7F;
            }
            func_80049CB0(SD_STATE->h157E);
            SD_STATE->h157E = -1;
        }
        {
            List *l = (List *)SD_STATE->p1564;
            u32 i = SD_STATE->h4E & 0xF;

            if (i >= l->count) {
                SD_STATE->b7C = 0;
                return;
            }
            SD_STATE->h157E = func_80049A64((u8 *)l + l->idx[i] * 16,
                                             SD_STATE->h157A);
            if ((s16)SD_STATE->h157E == 0) {
                SD_STATE->h157C = SD_STATE->h4E;
                func_80049AF4(1, 1);
                SD_STATE->b158A = 0xFF;
                SD_STATE->flags = SD_STATE->flags | 0x80;
                SD_STATE->h1588 = SD_STATE->h54;
            }
        }
        SD_STATE->b7C = 0;
        goto clear_7d;

    case 0:
        break;
    }

    goto clear_7d_7c;

clear_7d:
    SD_STATE->b7D = 0;
    return;

clear_7d_7c:
    SD_STATE->b7D = 0;
clear_7c:
    SD_STATE->b7C = 0;
}
