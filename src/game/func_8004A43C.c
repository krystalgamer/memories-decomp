#define D_8009B458_IN_DATA
#include "../types.h"
#include "sound.h"
#include "func_80049FB4.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"
#include "sound_spatialization.h"

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
