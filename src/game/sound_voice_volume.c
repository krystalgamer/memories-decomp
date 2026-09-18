#include "../types.h"
#include "../psyq/libspu.h"
#define D_80011434_IS_CONST
#define D_8009B458_IN_DATA
#include "sound.h"

/* Scales one secondary object's stereo level by the two channel masters at
 * +0x514/+0x516 and submits it as a volume-only SpuSetVoiceAttr request
 * through the shared block at D_8009B458 + 0x4C0. The mask is
 * VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels stay in
 * direct mode, and the 7.7 products are shifted back down by 7.
 *
 * The voice-mask table is const, as in sound_voice_envelope.c. A load from an
 * unchanging table does not depend on the stores through `s`, so the scheduler
 * is free to place it after the right-channel master is read: the table
 * address then occupies $v0 across that master's life, which puts the master
 * in $a1 and the first product in $a3, and keeps the mask value in $a0 next to
 * its store. With the plain declaration every source shape left the master in
 * $v0. The state-pointer arm keeps its address out of small data.
 */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    SDSecondaryState *s;

    s = D_8009B458;
    s->voice_attr.mask = SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    s->voice_attr.voice = D_80011434[voice];
    s->voice_attr.volume.left = (left * (u16)s->field_0514) >> 7;
    s->voice_attr.volume.right = (right * (u16)s->field_0516) >> 7;
    s->voice_attr.volmode.left = SPU_VOICE_DIRECT;
    s->voice_attr.volmode.right = SPU_VOICE_DIRECT;
    SpuSetVoiceAttr(&s->voice_attr);
}
