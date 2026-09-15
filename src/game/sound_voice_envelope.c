#include "../types.h"
#include "../psyq/libspu.h"
#define D_80011434_IS_CONST
#define D_8009B458_IN_DATA
#include "sound.h"

/* The voice-attribute block lives in the 0x40-byte hole at +0x4C0, which is
   exactly sizeof(SpuVoiceAttr) and pins a_mode/adsr1/adsr2 to the offsets the
   target stores through. The table and state-pointer declaration arms keep
   their addresses out of small data. */

void SD_SetVoiceEnvelopeFromTone(s32 index, SDToneEnvelopeView *tone)
{
    SDSecondaryState *p = D_8009B458;

    p->voice_attr.mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    p->voice_attr.voice = D_80011434[index];
    p->voice_attr.adsr1 = tone->adsr1;
    p->voice_attr.adsr2 = tone->adsr2;
    p->voice_attr.a_mode = tone->a_mode;
    SpuSetVoiceAttr(&p->voice_attr);
}

void SD_ResetVoiceEnvelope(s32 index)
{
    SDSecondaryState *p = D_8009B458;
    p->voice_attr.voice = D_80011434[index];
    p->voice_attr.mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    p->voice_attr.adsr1 = 0;
    p->voice_attr.adsr2 = 0;
    p->voice_attr.a_mode = SPU_VOICE_EXPIncN;
    SpuSetVoiceAttr(&p->voice_attr);
}
