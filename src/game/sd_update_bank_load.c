#include "../types.h"
#include "../psyq/stdio.h"
#include "../unmatched.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output.h"

/* MATCH 2026-09-06, first-day function from the m2c draft (248 instructions,
 * the sound bank loader state machine: SE header, SE groups, then the CD and
 * SMF mixer-out banks with their "VolInfo" signature checks). Jump table on
 * the state word, so only the full build proves it. Levers, in order: the
 * 8-byte header copied as ONE packed-struct assignment (retail batches the
 * two lwl/lwr pairs before the swl/swr pairs); the state increment written
 * INLINE in every arm rather than as a shared tail (retail cross-jumps the
 * identical ones itself); the shared SD_EnqueueRamTransfer call block placed
 * between cases 8 and 9 -- where retail has it -- so the return block is
 * preceded by the default arm and no store gets cross-jumped into it; case
 * 8's 0x3E7 arm out of line after that call block through a goto; the
 * scaled argument in case 2 as two statements against one name; the buffer
 * base assigned before the signature test; the two leading signature bytes
 * gSD_SeBankVolInfoTag/gSD_SmfBankVolInfoTag in .data (bare form beside the gp-relative
 * g_SDValue), which is why SDBankStagingBuffer starts at the second letter
 * rather than covering the whole word; and the first of its letters held in
 * a u8 local, whose zero-extension is the andi 0xFF retail keeps in a second
 * register for the last comparison.
 */

/* The European build makes none of the three printf calls. */
void SD_UpdateBankLoad(void) {
    SDBankStagingBuffer *b;
    u8 *a0;
    s32 a1;
    s32 a2;
    s32 a3;
    u8 c;

    switch (g_SDValue->bank_load_phase) {
    case 0:
        if (g_SDValue->flags_004A & 1) {
            a0 = (u8 *)0x801E6800;
            a1 = 0;
            a2 = 0x800;
            a3 = 0x10;
            goto call;
        }
        g_SDValue->bank_load_phase += 6;
        return;
    case 1:
        if ((SD_GetStatusFlags() & 7) == 0) {
            *(SDBankHeaderWords *)g_SDValue =
                *(SDBankHeaderWords *)0x801E6800;
            g_SDValue->bank_load_phase += 1;
            return;
        }
        return;
    case 2:
#ifndef VERSION_EUROPE
        printf(gSD_szSeGroupCountFormat, g_SDValue->value_link_count);
#endif
        a0 = (u8 *)0x801E2800;
        a1 = 1;
        a2 = g_SDValue->value_link_count;
        a3 = 0x10;
        a2 = a2 * 8;
        goto call;
    case 3:
        if ((SD_GetStatusFlags() & 7) == 0) {
            SD_InitVoiceState();
            g_SDValue->bank_load_phase += 1;
            return;
        }
        break;
    case 4:
        SD_QueueValueLinkTransfer(0);
        g_SDValue->bank_load_phase += 1;
        return;
    case 5:
        if (SD_GetStatusFlags() & 7) {
            return;
        }
        g_SDValue->bank_load_phase += 1;
        return;
    case 6:
        if (g_SDValue->flags_004A & 2) {
            a0 = (u8 *)0x801E2000;
            a1 = 0;
            a2 = 0x800;
            a3 = 0x20;
            goto call;
        }
        g_SDValue->bank_load_phase += 2;
        return;
    case 7:
        if ((SD_GetStatusFlags() & 7) == 0) {
            b = (SDBankStagingBuffer *)0x801E2000;
            if (gSD_SeBankVolInfoTag == 'V') {
                c = b->signature[0];
                if (c == 'o' && 'l' == b->signature[1] &&
                    'I' == b->signature[2] && 'n' == b->signature[3] &&
                    'f' == b->signature[4] && b->signature[5] == c) {
                    g_SDValue->secondary_mix_default = b->volume;
                    g_SDValue->secondary_mix_scale = g_SDValue->secondary_mix_default;
#ifndef VERSION_EUROPE
                    printf(gSD_szSmfMixerOutFormat, g_SDValue->secondary_mix_default);
#endif
                }
            }
            g_SDValue->bank_load_phase += 1;
            return;
        }
        break;
    case 8:
        if ((g_SDValue->flags_004A & 0x80) == 0) {
            goto no8;
        }
        a0 = (u8 *)0x801E8800;
        a1 = 0;
        a2 = 0x2000;
        a3 = 0x40;
    call:
        SD_EnqueueRamTransfer((s32)a0, a1, a2, a3);
        g_SDValue->bank_load_phase += 1;
        return;
    no8:
        g_SDValue->bank_load_phase = 0x3E7;
        return;
    case 9:
        if ((SD_GetStatusFlags() & 7) == 0) {
            b = (SDBankStagingBuffer *)0x801E8800;
            if (gSD_SmfBankVolInfoTag == 'V') {
                c = b->signature[0];
                if (c == 'o' && 'l' == b->signature[1] &&
                    'I' == b->signature[2] && 'n' == b->signature[3] &&
                    'f' == b->signature[4] && b->signature[5] == c) {
                    g_SDValue->cd_mix_default = b->volume;
                    g_SDValue->mix_scale = g_SDValue->cd_mix_default;
#ifndef VERSION_EUROPE
                    printf(gSD_szCdMixerOutFormat, g_SDValue->cd_mix_default);
#endif
                }
            }
            g_SDValue->cd_descriptor_banks[0] = (u8 *)0x801E8800;
            g_SDValue->cd_descriptor_banks[1] = (u8 *)0x801E9000;
            g_SDValue->cd_descriptor_banks[2] = (u8 *)0x801EA000;
            g_SDValue->bank_load_phase += 1;
            return;
        }
        break;
    default:
        if ((SD_GetStatusFlags() & 7) == 0) {
            g_SDValue->flags_0040 &= 0xFFF7;
        }
        break;
    }
    return;
}
