/* Reclassified from matching_c (#3859). This was
 * src/game/sound_voice_volume.c, byte-exact only under
 * gcc_2_8_1_cc_g8_as_g0_split, whose compiler and assembler disagree about
 * small data (GCC -G8, MASPSX -G0), and with 1 variable pinned to a hard
 * register. This source carries NO register pin. Under
 * gcc_2_8_1_cc_g8_as_g0_split it is 31 of 31 instructions, opcode census
 * distance 0, no structural blocks, and 28 of 31 aligned on opcode and
 * registers in 2 register-only blocks. */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"

/* Scales one secondary object's stereo level by the two channel masters at
 * +0x514/+0x516 and submits it as a volume-only SpuSetVoiceAttr request
 * through the shared block at D_8009B458 + 0x4C0. The mask is
 * VOLL|VOLR|VOLMODEL|VOLMODER; volmode is cleared so both channels stay in
 * direct mode, and the 7.7 products are shifted back down by 7.
 *
 * WHICH PROFILE EACH MEASUREMENT WAS TAKEN UNDER, because the previous
 * version of this comment did not say, and that omission cost 37 spellings.
 * Every shape it listed as dead -- reusing the dead `left` parameter, a u16
 * local, a separate local read before the first multiply, a u16 pointer
 * indexed [0]/[1], computing `r` before `l`, and both operand orders of the
 * second multiply -- was measured under gcc_2_8_1_g0_split, which does not
 * respond: under it every pin-free spelling ties at 23-24 of 31 whether or
 * not it is a good spelling. The same sources under
 * gcc_2_8_1_cc_g8_as_g0_split reach 26, so those rejections were taken on an
 * instrument that could not tell them apart and none of them is evidence.
 *
 * The one lever that moves it from 26 to 28 is the last two lines: the second
 * shift is assigned to `l`, which is dead after the +0x4C8 store, rather than
 * written inline. Eleven borrow candidates were measured under the profile
 * above -- `l` gives 28, `v` 25, `r` itself 24, a fresh local 26, and
 * borrowing any parameter (`left`, `right`, `voice`) collapses to 16, which
 * is the control that shows the axis is live rather than inert. Eleven
 * further axes on top of that base move nothing: do { } while (0) around the
 * shift pair, around both stores, around the second multiply, around the
 * voice store and around the mask store; the two volmode stores written
 * after the reads, split across the multiplies, or left in place; the voice
 * store written last; and `v` read early or late. Two tie at 28 and the rest
 * are worse.
 *
 * The residue is allocation only. Retail computes the second product into
 * $t1 and shifts it into $v0 (mflo $t1; sra $v0,$t1,7; sh $v0,0x4CA($v1));
 * this source uses $a1 and $a3 for the same three instructions. decomp-permuter
 * saturates against it: from the 26 base every stored output re-scores to
 * exactly this 28 when spliced into the real source, and from this 28 base
 * 8871 iterations produced no output at all. */
void SD_SetVoiceVolume(s32 voice, s32 left, s32 right) {
    u8 *b;
    s32 l;
    s32 r;
    s32 v;

    b = (u8 *)D_8009B458;
    l = left * *(u16 *)(b + 0x514);
    r = *(u16 *)(b + 0x516);
    r = right * r;
    *(s16 *)(b + 0x4CC) = SPU_VOICE_DIRECT;
    *(s16 *)(b + 0x4CE) = SPU_VOICE_DIRECT;
    v = D_80011434[voice];
    *(s32 *)(b + 0x4C4) =
        SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_VOLMODEL | SPU_VOICE_VOLMODER;
    *(s32 *)(b + 0x4C0) = v;
    *(s16 *)(b + 0x4C8) = l >> 7;
    l = r >> 7;
    *(s16 *)(b + 0x4CA) = l;
    SpuSetVoiceAttr((SpuVoiceAttr *)(b + 0x4C0));
}
