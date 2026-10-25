#define G_SDVALUE_IN_DATA
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_output_state.h"

#include "sound_voice_selection.h"
#include "sound_voice_constants.h"
#include "sound_voice_allocator.h"

void SD_AllocateSoundEffectVoice(s32 id, s16 pitch, u8 volume, s16 pan, u32 mode, u8 value)
{
    u16 sound_id = id;
    u8 slot;
    u8 count;
    u8 found;
    s32 index;
    s32 mask;
    s32 observed;
    u8 occurrence;
    s16 envelope;
    s16 mode_snapshot;

    mode_snapshot = mode;
    if (g_SDValue->field_043C[(u16)sound_id] == 0xFFFF) {
        return;
    }
    if ((id & 0x8000) != 0) {
        SD_EnqueueCdSePlay(sound_id, 0);
        return;
    }

    found = 0;
    if ((mode & 0xF) != 0) {
        mask = SD_KeyOffVoiceGroup(mode & 0xF);
        if (mask != 0) {
            slot = 0;
            do {
                if (mask & 1) {
                    g_SDValue->voice_allocation_cursor = slot;
                    SD_StartSoundEffectVoice(sound_id, g_SDValue->voice_allocation_cursor, pitch, volume,
                                  pan, mode_snapshot, value);
                    return;
                }
                slot++;
                mask >>= 1;
            } while (slot < SD_VOICE_SLOT_COUNT);
        }
    }

    if (((u16)mode_snapshot & 0xF0) != 0) {
        slot = 0;
        count = 0;
        occurrence = ((u16)mode_snapshot & 0xF0) >> 4;
        for (index = 0; index < SD_VOICE_SLOT_COUNT; index++, slot++) {
            SpuGetVoiceEnvelope(
                index + SD_VOICE_SLOT_FIRST_VOICE, &envelope
            );
            /* Sharing the scratch keeps ID narrowing inside this loop. */
            observed = envelope;
            if (observed != 0) {
                observed = (u16)sound_id;
                if (g_SDValue->voice_ids[slot] == observed) {
                    count++;
                    if (count == occurrence) {
                        g_SDValue->voice_allocation_cursor = slot;
                        SD_StartSoundEffectVoice(sound_id, g_SDValue->voice_allocation_cursor, pitch, volume,
                                      pan, mode_snapshot, value);
                        return;
                    }
                }
            }
        }
    }

    mask = 1 << g_SDValue->voice_allocation_cursor;
    if (found == 0) {
        index = 0;
        do {
            SpuGetVoiceEnvelope(
                g_SDValue->voice_allocation_cursor + SD_VOICE_SLOT_FIRST_VOICE, &envelope
            );
            if (envelope == 0) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->voice_allocation_cursor, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->voice_allocation_cursor = g_SDValue->voice_allocation_cursor + 1;
            g_SDValue->voice_allocation_cursor = g_SDValue->voice_allocation_cursor & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }

    if (found == 0) {
        index = 0;
        mask = 1 << g_SDValue->voice_allocation_cursor;
        do {
            if (value >= g_SDValue->voice_priority[g_SDValue->voice_allocation_cursor]) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->voice_allocation_cursor, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->voice_allocation_cursor = g_SDValue->voice_allocation_cursor + 1;
            g_SDValue->voice_allocation_cursor = g_SDValue->voice_allocation_cursor & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }
}
