/* Reclassified from matching_c (#3859). Under the valid uniform
 * gcc_2_8_1_g0_split profile this binding-free source is 31 of 31
 * instructions with eight differing words and opcode distance 0. */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"

/* Scales one secondary object's stereo level by the two channel masters at
 * +0x514/+0x516 and submits it as a volume-only SpuSetVoiceAttr request
 * through the shared block at D_8009B458 + 0x4C0. The mask is
 * VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels stay in
 * direct mode, and the 7.7 products are shifted back down by 7.
 *
 * Chaining the right-channel master through `r` keeps that load and multiply
 * in retail's $a1. The remaining differences are the root pointer's split
 * address register and the two multiply-result registers.
 */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    u8 *b;
    s32 master_right;
    s32 l;
    s32 r;
    s32 v;

    b = (u8 *)D_8009B458;
    l = left * *(u16 *)(b + 0x514);
    master_right = (r = *(u16 *)(b + 0x516));
    r = right * r;
    *(s16 *)(b + 0x4CC) = SPU_VOICE_DIRECT;
    *(s16 *)(b + 0x4CE) = SPU_VOICE_DIRECT;
    v = D_80011434[voice];
    *(s32 *)(b + 0x4C4) =
        SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    *(s32 *)(b + 0x4C0) = v;
    *(s16 *)(b + 0x4C8) = l >> 7;
    *(s16 *)(b + 0x4CA) = r >> 7;
    SpuSetVoiceAttr((SpuVoiceAttr *)(b + 0x4C0));
}
