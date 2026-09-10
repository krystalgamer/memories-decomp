/* Reclassified from matching_c (#3859). This was part of
 * src/game/sound_voice_setup.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0, a single threshold,
 * it is 54 instructions against the target's 55, opcode distance 3. The
 * source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../game/sound.h"
#include "../game/func_80049FB4.h"
#include "../unmatched.h"
#include "../psyq/libspu.h"
#include "../game/sound_sequence_constants.h"
#include "../game/sound_spatialization.h"

/* Re-derives a voice's raw pitch when its channel's pitch-bend MSB changes
 * (or when forced). Caches the bend value in the object, adds SD_CalcPitchBend's
 * adjustment to the note in 7.7 units, fills the voice_attr request block
 * in the secondary state and submits it through SpuSetVoiceAttr. */
void func_8004A43C(SDSecondaryObject *p, s32 force) {
    u8 *e;
    s32 v;
    s32 x;
    s32 y;

    e = (u8 *)D_8009B458 +
        p->channel_index * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    if (e[7] == p->cached_pitch_bend && force == 0) {
        return;
    }
    p->cached_pitch_bend = e[7];
    v = SD_CalcPitchBend(p, e[7]);
    v = v + (p->note << 7);
    x = (s16)v >> 7;
    y = v & 0x7F;
    D_8009B458->voice_attr.mask = SPU_VOICE_PITCH;
    D_8009B458->voice_attr.voice = D_80011434[p->voice_index];
    v = func_80049FB4(x, y, p->field_0012, p->field_0013);
    D_8009B458->voice_attr.pitch = v;
    SpuSetVoiceAttr(&D_8009B458->voice_attr);
}
