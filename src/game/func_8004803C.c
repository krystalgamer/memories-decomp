#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    u8 volume;
    u8 timer;
    u8 pad2[2];
    u16 pitch;
    u16 field_6;
} SDVoiceEntry;

typedef struct {
    u8 pad000[0x384];
    u32 key_mask;
    u8 pad388[4];
    s16 volume_left;
    s16 volume_right;
    u8 pad390[0xA];
    u16 pitch;
    u8 pad39C[4];
    u32 field_3A0;
    u8 pad3A4[0x60];
    u16 voice_ids[SD_VOICE_SLOT_COUNT];
    u8 field_40C[SD_VOICE_SLOT_COUNT];
    u8 voice_flags[SD_VOICE_SLOT_COUNT];
    u16 voice_volume_left[SD_VOICE_SLOT_COUNT];
    u16 voice_volume_right[SD_VOICE_SLOT_COUNT];
    u8 voice_value[SD_VOICE_SLOT_COUNT];
    u8 voice_step[SD_VOICE_SLOT_COUNT];
    u16 voice_timer[SD_VOICE_SLOT_COUNT];
    u8 voice_active_mask;
    u8 pad435[7];
    u16 *voice_index;
    u8 pad440[4];
    SDVoiceEntry *entries;
} SDVoiceState;

#define SD ((SDVoiceState *)g_SDValue)

/* Starts a sound-effect voice: looks the sound id up in the voice index table,
   derives the left and right volumes from the note table entry and the pan,
   keys the voice on and records the id, volumes and timer in the voice slots.
   The voice fields sit in the unnamed part of SDValue, so they are described
   by a local overlay of the same layout. */

void func_8004803C(u16 id, u8 voice, s32 pitch_add, u8 volume, s16 pan, u8 flags, u8 value) {
    u16 idx;

    idx = SD->voice_index[id];
    if (idx != 0xFFFF) {
        SD->volume_left = ((SD->entries[idx].volume * volume) >> 1) | 0xFF;
        SD->volume_right = ((SD->entries[idx].volume * volume) >> 1) | 0xFF;
        if (pan != 0) {
            if ((u16)(pan - 1) < 0x80) {
                SD->volume_left = SD->volume_left * (0x80 - pan) / 128;
            }
            if ((u16)(pan + 0x80) < 0x80) {
                SD->volume_right = SD->volume_right * (pan + 0x80) / 128;
            }
        }
        SD->pitch = SD->entries[idx].pitch + pitch_add;
        SD->key_mask = 1 << (voice + 0x14);
        SD->field_3A0 = SD->entries[idx].field_6 << 4;
        SpuSetKey(0, SD->key_mask);
        SD->voice_active_mask |= 1 << voice;
        SD->field_40C[voice] = value;
        SD->voice_flags[voice] = flags;
        SD->voice_ids[voice] = id;
        SD->voice_volume_left[voice] = SD->volume_left;
        SD->voice_volume_right[voice] = SD->volume_right;
        SD->voice_value[voice] = 0xFF;
        SD->voice_step[voice] = 0;
        SD->voice_timer[voice] = SD->entries[idx].timer << 2;
        SpuSetKeyOnWithAttr((SpuVoiceAttr *)&SD->key_mask);
    }
}
