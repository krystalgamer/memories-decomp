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
    SDBankStagingBuffer *bank_tail;
    u8 *ram_destination;
    s32 sector_offset;
    s32 ram_size;
    s32 source_selector;
    u8 signature_first;

    switch (g_SDValue->bank_load_phase) {
    case 0:
        if (g_SDValue->data_source_flags & 1) {
            ram_destination = (u8 *)0x801E6800;
            sector_offset = 0;
            ram_size = 0x800;
            source_selector = 0x10;
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
        ram_destination = (u8 *)0x801E2800;
        sector_offset = 1;
        ram_size = g_SDValue->value_link_count;
        source_selector = 0x10;
        ram_size = ram_size * 8;
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
        if (g_SDValue->data_source_flags & 2) {
            ram_destination = (u8 *)0x801E2000;
            sector_offset = 0;
            ram_size = 0x800;
            source_selector = 0x20;
            goto call;
        }
        g_SDValue->bank_load_phase += 2;
        return;
    case 7:
        if ((SD_GetStatusFlags() & 7) == 0) {
            bank_tail = (SDBankStagingBuffer *)0x801E2000;
            if (gSD_SeBankVolInfoTag == 'V') {
                signature_first = bank_tail->signature[0];
                if (signature_first == 'o' && 'l' == bank_tail->signature[1] &&
                    'I' == bank_tail->signature[2] && 'n' == bank_tail->signature[3] &&
                    'f' == bank_tail->signature[4] && bank_tail->signature[5] == signature_first) {
                    g_SDValue->secondary_mix_default = bank_tail->volume;
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
        if ((g_SDValue->data_source_flags & 0x80) == 0) {
            goto no8;
        }
        ram_destination = (u8 *)0x801E8800;
        sector_offset = 0;
        ram_size = 0x2000;
        source_selector = 0x40;
    call:
        SD_EnqueueRamTransfer((s32)ram_destination, sector_offset, ram_size, source_selector);
        g_SDValue->bank_load_phase += 1;
        return;
    no8:
        g_SDValue->bank_load_phase = 0x3E7;
        return;
    case 9:
        if ((SD_GetStatusFlags() & 7) == 0) {
            bank_tail = (SDBankStagingBuffer *)0x801E8800;
            if (gSD_SmfBankVolInfoTag == 'V') {
                signature_first = bank_tail->signature[0];
                if (signature_first == 'o' && 'l' == bank_tail->signature[1] &&
                    'I' == bank_tail->signature[2] && 'n' == bank_tail->signature[3] &&
                    'f' == bank_tail->signature[4] && bank_tail->signature[5] == signature_first) {
                    g_SDValue->cd_mix_default = bank_tail->volume;
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
            g_SDValue->status_flags &= 0xFFF7;
        }
        break;
    }
    return;
}
