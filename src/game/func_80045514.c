#define G_SDVALUE_IN_DATA
#define SD_SECONDARY_STEPS_TAKE_AMBIENT_ARG
#define SOUND_TRANSFER_REQUEST_IN_DATA
#include "../types.h"
#include "sound_transfer_lifecycle.h"
#include "file_transfer.h"
#include "sound.h"
#include "sound_pending_entries.h"
#include "sound_output_state.h"
#include "sound_sequence_timing.h"
#include "../ygo_types.h"

#include "sound_command_index.h"

void func_80045514(void)
{
    SDValue *entry_state;
    u32 first_word;
    u32 second_word;

    switch (g_SDValue->field_007C) {
    case 81:
        func_800476B4((SDSeqBlock *)g_SDValue->field_0058,
                     (u32)g_SDValue->field_0050);
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
                s32 final_word;

                b->field_10 = (s32)sd->field_005C[4];
                b->field_0C = (s32)sd->field_005C[3];
                b->field_04 = (s32)sd->field_005C[1];
                b->field_18 = (s32)sd->field_005C[6];
                final_word = (s32)sd->field_005C[5];
                b->field_1F = 0;
                b->field_1C = 0;
                b->field_14 = final_word;
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
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
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
        if ((SD_HasQueuedStreamCommand() & 0xFF) != 0) {
            goto clear_7d_7c;
        }
        {
            SDValue *sd = g_SDValue;
            u16 flags = sd->flags_0040;
            sd->field_007D = 0;
            sd->flags_0040 = flags & 0xFFF8;
            g_SDValue->field_007C = 0;
        }
        return;

    case 33:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            SoundCommandPair *e;
            s32 base_offset;

            first_word = 0xFFFFFF;
            e = (SoundCommandPair *)(
                (u32)&((SoundCommandPair *)0)[g_SDValue->field_004E]
                + (u32)g_SDValue->field_0058);
            second_word = e->a;

            D_8009B460->field_10 = 0;
            D_8009B460->field_18 = 0;
            D_8009B460->field_0C = 0;
            D_8009B460->field_14 = 0;
            D_8009B460->field_1C = 0;
            D_8009B460->field_1F = g_SDValue->field_0530;
            second_word &= first_word;
            D_8009B460->field_1E = g_SDValue->field_0531;
            base_offset = (s32)g_SDValue->field_0050;
            D_8009B460->field_00 = 6;
            second_word += base_offset;
            D_8009B460->field_04 = second_word;
            func_80014C40(D_8009B460, 0);
        }
        goto clear_7d_7c;

    case 36:
        if (func_80014C40(0, 0) != 0) {
            goto clear_7d_7c;
        }
        {
            u32 value_mask = 0xFFFFFF;
            SoundCommandPair *e;
            s32 request_offset;
            u32 low_a;

            entry_state = g_SDValue;
            e = (SoundCommandPair *)((u32)entry_state->field_004E * 8
                                     + (u32)entry_state->field_0058);
            first_word = e->a;
            second_word = e->b;

            low_a = first_word & value_mask;
            value_mask &= second_word;
            entry_state->field_0528 = low_a;
            entry_state->field_0531 = (first_word & 0x1F000000) >> 24;
            entry_state->field_052C = value_mask;
            g_SDValue->field_0530 = (second_word & 0x1F000000) >> 24;
            second_word >>= 31;
            g_SDValue->field_0532 = second_word;
            first_word >>= 29;
            g_SDValue->mix_multiplier = first_word;
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
            request_offset = g_SDValue->field_0528;
            D_8009B460->field_00 = 6;
            D_8009B460->field_04 = request_offset;
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
            SDMusicPackage *list = (SDMusicPackage *)g_SDValue->music_track;

            g_SDValue->field_157A = SD_VabOpenHead(
                (SDVabHeader *)list->vab_header, 0, list->spu_address
            );
            if ((s16)g_SDValue->field_157A != 0) {
                g_SDValue->field_007C = 0;
                return;
            }
        }
        if (g_SDValue->field_157E == 0) {
            if ((g_SDValue->flags_0040 & 0x80) != 0) {
                SD_StopSequence(0);
                func_80049F10(0, 0);
                g_SDValue->field_1586 = 0;
                g_SDValue->field_1588 = 0;
                g_SDValue->flags_0040 = g_SDValue->flags_0040 & 0xFF7F;
            }
            func_80049CB0(g_SDValue->field_157E);
            g_SDValue->field_157E = -1;
        }
        {
            CommandIndexTable *l;
            u32 i;

            entry_state = g_SDValue;
            l = (CommandIndexTable *)entry_state->music_track;
            i = entry_state->field_004E & 0xF;

            if (i >= l->count) {
                g_SDValue->field_007C = 0;
                return;
            }
            g_SDValue->field_157E = SD_OpenSequence(
                (u8 *)((u32)l->index_words[i * 2] * 16 + (u32)l),
                                             entry_state->field_157A);
            if ((s16)g_SDValue->field_157E == 0) {
                g_SDValue->field_157C = g_SDValue->field_004E;
                SD_PlaySequence(1, 1);
                {
                    SDValue *sd = g_SDValue;
                    sd->field_158A = 0xFF;
                    sd->flags_0040 = sd->flags_0040 | 0x80;
                    sd->field_1588 = (u16)sd->field_0054;
                }
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
