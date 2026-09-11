#include "../types.h"
#include "../psyq/libspu.h"
#define D_80011434_IS_CONST
#define D_8009B458_IN_DATA
#include "sound.h"

/* The voice-attribute block lives in the 0x40-byte hole at +0x4C0, which is
   exactly sizeof(SpuVoiceAttr) and pins a_mode/adsr1/adsr2 to the offsets the
   target stores through. The table and state-pointer declaration arms keep
   their addresses out of small data. */

#define ATTR(p) (*(SpuVoiceAttr *)((u8 *)(p) + 0x4C0))

void func_8004A764(s32 index)
{
    u8 *p = (u8 *)D_8009B458;
    ATTR(p).voice = D_80011434[index];
    ATTR(p).mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    ATTR(p).adsr1 = 0;
    ATTR(p).adsr2 = 0;
    ATTR(p).a_mode = SPU_VOICE_EXPIncN;
    SpuSetVoiceAttr(&ATTR(p));
}
