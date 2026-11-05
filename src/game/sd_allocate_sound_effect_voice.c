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
    if (g_SDValue->s_pwSD_SE_INDEX[(u16)sound_id] == 0xFFFF) {
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
                    g_SDValue->s_bSD_VOICE_CURSOR = slot;
                    SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume,
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
                if (g_SDValue->s_awSD_VOICE_SE_ID[slot] == observed) {
                    count++;
                    if (count == occurrence) {
                        g_SDValue->s_bSD_VOICE_CURSOR = slot;
                        SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume,
                                      pan, mode_snapshot, value);
                        return;
                    }
                }
            }
        }
    }

    mask = 1 << g_SDValue->s_bSD_VOICE_CURSOR;
    if (found == 0) {
        index = 0;
        do {
            SpuGetVoiceEnvelope(
                g_SDValue->s_bSD_VOICE_CURSOR + SD_VOICE_SLOT_FIRST_VOICE, &envelope
            );
            if (envelope == 0) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR + 1;
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }

    if (found == 0) {
        index = 0;
        mask = 1 << g_SDValue->s_bSD_VOICE_CURSOR;
        do {
            if (value >= g_SDValue->s_abSD_VOICE_PRIORITY[g_SDValue->s_bSD_VOICE_CURSOR]) {
                found = 1;
                SD_StartSoundEffectVoice(sound_id, g_SDValue->s_bSD_VOICE_CURSOR, pitch, volume, pan,
                              mode_snapshot, value);
            }
            mask <<= 1;
            if ((mask & 0x10) != 0) {
                mask = 1;
            }
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR + 1;
            g_SDValue->s_bSD_VOICE_CURSOR = g_SDValue->s_bSD_VOICE_CURSOR & 3;
            index++;
        } while (index < SD_VOICE_SLOT_COUNT && found == 0);
    }
}
