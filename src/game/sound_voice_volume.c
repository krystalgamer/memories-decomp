#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

/* Scales one secondary object's stereo level by the two channel masters at
 * +0x514/+0x516 and submits it as a volume-only SpuSetVoiceAttr request
 * through the shared block at D_8009B458 + 0x4C0. The mask is
 * VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels stay in
 * direct mode, and the 7.7 products are shifted back down by 7.
 *
 * The right-channel master is pinned to $a1. Every source shape tried left it
 * in $v0 and the product in $a1, which also sank the voice store below the
 * volume.left store: reusing the dead `left` parameter to hold it, a u16
 * local, a separate local read before the first multiply, a u16 pointer
 * indexed [0]/[1], computing `r` before `l`, and both operand orders of the
 * second multiply. All nine gave the same allocation and the same swap.
 */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    u8 *b;
    register s32 master_right asm("$5");
    s32 l;
    s32 r;
    s32 v;

    b = (u8 *)D_8009B458;
    l = left * *(u16 *)(b + 0x514);
    master_right = *(u16 *)(b + 0x516);
    r = right * master_right;
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
