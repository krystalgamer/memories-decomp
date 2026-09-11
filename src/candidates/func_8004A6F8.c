/* Reclassified from matching_c (#3859). This was part of
 * src/game/sound_voice_envelope.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0). Under gcc_2_8_1_g0_split, a single
 * threshold, it is 27 of 27 instructions with 5 differing, opcode distance
 * 0. The source below is the match, unchanged apart from its include paths. */
#include "../types.h"
#include "../psyq/libspu.h"
#define D_80011434_IS_CONST
#include "../game/sound.h"
#include "../unmatched.h"

/* The voice-attribute block lives in the 0x40-byte hole at +0x4C0, which is
   exactly sizeof(SpuVoiceAttr) and pins a_mode/adsr1/adsr2 to the offsets the
   target stores through. `const` on the table keeps it out of small data so
   its address is materialised rather than reached through $gp. */

#define ATTR(p) (*(SpuVoiceAttr *)((u8 *)(p) + 0x4C0))

/* The supplied tone record is not a Psy-Q VagAtr, so its fields stay byte
   offsets until something names them. */
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
