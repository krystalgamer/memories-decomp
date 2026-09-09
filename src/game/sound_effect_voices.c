#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

/* The sound-effect voice slots: the active-voice count for one sound id
   (0x80047FAC) and the voice start that plays one (0x8004803C). The two are
   contiguous -- 0x80047FAC is 0x90 bytes and runs straight into 0x8004803C --
   and they are the whole gcc_2_8_1_cc_g0_as_g8_no_split run between
   sound_voice_selection.c and func_80048768. They are the reader and the
   writer of the same four voice slots: the count walks g_SDValue->voice_ids
   and the start assigns it, along with the flags, volumes and timer beside
   it. */

/* Counts how many of the four voice slots are playing this sound id: an id
   match in voice_ids paired with a non-zero SPU envelope, which is what
   distinguishes a slot still sounding from one that has already decayed. */
s16 func_80047FAC(s32 a0) {
    u16 s1 = 0;
    s32 s0;
    u16 s2 = a0 & 0xFFFF;

    for (s0 = 0; s0 < SD_VOICE_SLOT_COUNT; s0++) {
        s16 local;

        SpuGetVoiceEnvelope(s0 + SD_VOICE_SLOT_FIRST_VOICE, &local);

        if (g_SDValue->voice_ids[s0] == s2) {
            if (local != 0) {
                s1++;
            }
        }
    }

    return (s16)s1;
}

/* Starts a sound-effect voice: looks the sound id up in the voice index table,
   derives the left and right volumes from the note table entry and the pan,
   keys the voice on and records the id, volumes and timer in the voice slots. */
void func_8004803C(u16 id, u8 voice, s32 pitch_add, u8 volume, s16 pan, u8 flags, u8 value) {
    u16 idx;

    idx = g_SDValue->field_043C[id];
    if (idx != 0xFFFF) {
        g_SDValue->voice_attr.volume.left = ((g_SDValue->field_0444[idx].volume * volume) >> 1) | 0xFF;
        g_SDValue->voice_attr.volume.right = ((g_SDValue->field_0444[idx].volume * volume) >> 1) | 0xFF;
        if (pan != 0) {
            if ((u16)(pan - 1) < 0x80) {
                g_SDValue->voice_attr.volume.left = g_SDValue->voice_attr.volume.left * (0x80 - pan) / 128;
            }
            if ((u16)(pan + 0x80) < 0x80) {
                g_SDValue->voice_attr.volume.right = g_SDValue->voice_attr.volume.right * (pan + 0x80) / 128;
            }
        }
        g_SDValue->voice_attr.note = g_SDValue->field_0444[idx].pitch + pitch_add;
        g_SDValue->voice_attr.voice = 1 << (voice + SD_VOICE_SLOT_KEY_SHIFT);
        g_SDValue->voice_attr.addr = g_SDValue->field_0444[idx].field_0006 << 4;
        SpuSetKey(SPU_OFF, g_SDValue->voice_attr.voice);
        g_SDValue->voice_active_mask |= 1 << voice;
        g_SDValue->field_040C[voice] = value;
        g_SDValue->voice_flags[voice] = flags;
        g_SDValue->voice_ids[voice] = id;
        g_SDValue->voice_volume_left[voice] = g_SDValue->voice_attr.volume.left;
        g_SDValue->voice_volume_right[voice] = g_SDValue->voice_attr.volume.right;
        g_SDValue->voice_value[voice] = 0xFF;
        g_SDValue->voice_step[voice] = 0;
        g_SDValue->voice_timer[voice] = g_SDValue->field_0444[idx].timer << 2;
        SpuSetKeyOnWithAttr(&g_SDValue->voice_attr);
    }
}
