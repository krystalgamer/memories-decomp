#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

/* Same voice-attribute block at +0x4C0 as func_8004A764, but the envelope
   values come from a caller-supplied tone record rather than constants. The
   record is not a Psy-Q VagAtr, so its fields stay byte offsets until
   something names them. */
extern const s32 D_80011434[];

#define ATTR(q) (*(SpuVoiceAttr *)((u8 *)(q) + 0x4C0))

void func_8004A6F8(s32 index, u8 *tone)
{
    u8 *p = (u8 *)D_8009B458;
    ATTR(p).mask =
        SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_ADSR1 | SPU_VOICE_ADSR_ADSR2;
    ATTR(p).voice = D_80011434[index];
    ATTR(p).adsr1 = *(u16 *)(tone + 0x20);
    ATTR(p).adsr2 = *(u16 *)(tone + 0x22);
    ATTR(p).a_mode = *(u16 *)(tone + 0x24);
    SpuSetVoiceAttr(&ATTR(p));
}
