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
#include "../ygo_types.h"

extern void func_800476B4(u8 *, s32);
extern u8 func_80045484(void);
extern void SD_ArmBusyCallback(void);
extern s16 func_80049A64(u8 *, s16);
extern void func_80049AF4(s32, s32);

void func_80045514(void)
{
    switch (g_SDValue->field_007C) {
    case 81:
        func_800476B4((u8 *)g_SDValue->field_0058, (s32)g_SDValue->field_0050);
        goto clear_7d_7c;

    case 32:
        switch (g_SDValue->field_007D) {
        case 0:
            if (func_80014C40(0, 0) != 0) {
                return;
            }
            if (g_SDValue->field_04CC != 0) {
                return;
            }
            {
                SDValue *sd = g_SDValue;
                FileRequestSlot *b = D_8009B460;

                b->field_10 = (s32)sd->field_005C[4];
                b->field_0C = (s32)sd->field_005C[3];
                b->field_04 = (s32)sd->field_005C[1];
                b->field_18 = (s32)sd->field_005C[6];
                b->field_1F = 0;
                b->field_1C = 0;
                b->field_14 = (s32)sd->field_005C[5];
                D_8009B460->field_1E = 0;
            }
            D_8009B460->field_00 = (s32)g_SDValue->field_005C[0];
            if (D_8009B460->field_18 == 0 && D_8009B460->field_14 == 0) {
                g_SDValue->field_007D = 0;
                goto clear_7c;
            }
            switch ((s32)g_SDValue->field_005C[0] & 0xF0) {
            case 0x10:
                D_8009B460->field_00 = 4;
                func_80014C40(D_8009B460, g_SDValue->field_1629);
                break;
            case 0x20:
                D_8009B460->field_00 = 5;
                func_80014C40(D_8009B460, g_SDValue->field_1619);
                break;
            case 0x40:
                D_8009B460->field_00 = 6;
                func_80014C40(D_8009B460, g_SDValue->field_1639);
                break;
            }
            SD_ArmBusyCallback();
            g_SDValue->field_007D = g_SDValue->field_007D + 1;
            return;
        case 1:
            break;
        default:
            return;
        }
        if (g_SDValue->busy != 0) {
            return;
        }
        g_SDValue->field_007D = 0;
        g_SDValue->field_007C = 0;
        if ((func_80045484() & 0xFF) != 0) {
            return;
        }
        g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFFFC;
        return;

    case 17:
        switch (g_SDValue->field_007D) {
        case 0:
            if (g_SDValue->cd_volume == 0) {
                g_SDValue->field_007D = 1;
                return;
            }
            if (g_SDValue->field_0512 < 0) {
                return;
            }
            g_SDValue->field_0512 = -0x20;
            g_SDValue->field_0049 = 0;
            return;
        case 1:
            break;
        default:
            return;
        }
        g_SDValue->field_0534 = 0xFFFF;
        if ((func_80045484() & 0xFF) != 0) {
            goto clear_7d_7c;
        }
        g_SDValue->field_007D = 0;
        g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFFF8;
        g_SDValue->field_007C = 0;
        return;

    case 33:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e =
                (SoundCommandPair *)((u8 *)g_SDValue->field_0058
                                     + g_SDValue->field_004E * 8);

            D_8009B460->field_10 = 0;
            D_8009B460->field_18 = 0;
            D_8009B460->field_0C = 0;
            D_8009B460->field_14 = 0;
            D_8009B460->field_1C = 0;
            D_8009B460->field_1F = g_SDValue->field_0530;
            D_8009B460->field_1E = g_SDValue->field_0531;
            D_8009B460->field_00 = 6;
            D_8009B460->field_04 =
                (e->a & 0xFFFFFF) + (s32)g_SDValue->field_0050;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 36:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e =
                (SoundCommandPair *)((u8 *)g_SDValue->field_0058
                                     + g_SDValue->field_004E * 8);
            u32 a = e->a;
            u32 b = e->b;

            g_SDValue->field_0528 = a & 0xFFFFFF;
            g_SDValue->field_0531 = (a & 0x1F000000) >> 24;
            g_SDValue->field_052C = b & 0xFFFFFF;
            g_SDValue->field_0530 = (b & 0x1F000000) >> 24;
            g_SDValue->field_0532 = b >> 31;
            g_SDValue->mix_multiplier = a >> 29;
            g_SDValue->field_0528 =
                g_SDValue->field_0528 + (s32)g_SDValue->field_0050;
            g_SDValue->field_052C =
                g_SDValue->field_052C + (s32)g_SDValue->field_0050;
            D_8009B460->field_10 = 0;
            D_8009B460->field_18 = 0;
            D_8009B460->field_0C = 0;
            D_8009B460->field_14 = 0;
            D_8009B460->field_1C = (u16)g_SDValue->field_052C
                                   - (u16)g_SDValue->field_0528 + 0x10;
            D_8009B460->field_1F = g_SDValue->field_0530;
            D_8009B460->field_1E = g_SDValue->field_0531;
            D_8009B460->field_00 = 6;
            D_8009B460->field_04 = g_SDValue->field_0528;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 41:
    case 42:
        if (g_SDValue->field_007E != 0 && g_SDValue->field_0512 != 0) {
            return;
        }
        g_SDValue->field_007C = 0;
        goto clear_7d;

    case 72:
        if (g_SDValue->field_157A != 0) {
            u8 *list = (u8 *)g_SDValue->music_track;

            g_SDValue->field_157A = func_800496C4(list + 0x50, 0,
                                             *(s32 *)(list + 0xC));
            if ((s16)g_SDValue->field_157A != 0) {
                g_SDValue->field_007C = 0;
                return;
            }
        }
        if (g_SDValue->field_157E == 0) {
            if ((g_SDValue->flags_0040 & 0x80) != 0) {
                func_80049C40(0);
                func_80049F10(0, 0);
                g_SDValue->field_1586 = 0;
                g_SDValue->field_1588 = 0;
                g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFF7F;
            }
            func_80049CB0(g_SDValue->field_157E);
            g_SDValue->field_157E = -1;
        }
        {
            SoundIndexList *l = (SoundIndexList *)g_SDValue->music_track;
            u32 i = g_SDValue->field_004E & 0xF;

            if (i >= l->count) {
                g_SDValue->field_007C = 0;
                return;
            }
            g_SDValue->field_157E = func_80049A64(
                (u8 *)l + l->indices[i] * 16,
                                             g_SDValue->field_157A);
            if ((s16)g_SDValue->field_157E == 0) {
                g_SDValue->field_157C = g_SDValue->field_004E;
                func_80049AF4(1, 1);
                g_SDValue->field_158A = 0xFF;
                g_SDValue->flags_0040 = g_SDValue->flags_0040 | 0x80;
                g_SDValue->field_1588 = (u16)g_SDValue->field_0054;
            }
        }
        g_SDValue->field_007C = 0;
        goto clear_7d;

    case 0:
        break;
    }

    goto clear_7d_7c;

clear_7d:
    g_SDValue->field_007D = 0;
    return;

clear_7d_7c:
    g_SDValue->field_007D = 0;
clear_7c:
    g_SDValue->field_007C = 0;
}
