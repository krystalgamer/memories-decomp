#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

/* Starts a sound-effect voice: looks the sound id up in the voice index table,
   derives the left and right volumes from the note table entry and the pan,
   keys the voice on and records the id, volumes and timer in the voice slots. */
void func_8004803C(u16 id, u8 voice, s32 pitch_add, u8 volume, s16 pan, u8 flags, u8 value) {
    u16 idx;

    idx = g_SDValue->field_043C[id];
    if (idx != 0xFFFF) {
        g_SDValue->volume_left = ((g_SDValue->field_0444[idx].volume * volume) >> 1) | 0xFF;
        g_SDValue->volume_right = ((g_SDValue->field_0444[idx].volume * volume) >> 1) | 0xFF;
        if (pan != 0) {
            if ((u16)(pan - 1) < 0x80) {
                g_SDValue->volume_left = g_SDValue->volume_left * (0x80 - pan) / 128;
            }
            if ((u16)(pan + 0x80) < 0x80) {
                g_SDValue->volume_right = g_SDValue->volume_right * (pan + 0x80) / 128;
            }
        }
        g_SDValue->pitch = g_SDValue->field_0444[idx].pitch + pitch_add;
        g_SDValue->key_mask = 1 << (voice + 0x14);
        g_SDValue->field_03A0 = g_SDValue->field_0444[idx].field_0006 << 4;
        SpuSetKey(0, g_SDValue->key_mask);
        g_SDValue->voice_active_mask |= 1 << voice;
        g_SDValue->field_040C[voice] = value;
        g_SDValue->voice_flags[voice] = flags;
        g_SDValue->voice_ids[voice] = id;
        g_SDValue->voice_volume_left[voice] = g_SDValue->volume_left;
        g_SDValue->voice_volume_right[voice] = g_SDValue->volume_right;
        g_SDValue->voice_value[voice] = 0xFF;
        g_SDValue->voice_step[voice] = 0;
        g_SDValue->voice_timer[voice] = g_SDValue->field_0444[idx].timer << 2;
        SpuSetKeyOnWithAttr((SpuVoiceAttr *)&g_SDValue->key_mask);
    }
}
