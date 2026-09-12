/*
 * func_80047864: voice-slot attribute initialization order
 *
 * The 136-byte routine at `0x80047864` matches all 34 instructions with the
 * existing uniform `gcc_2_8_1_g0` profile. It uses the incoming index directly
 * and the shared `SDValue` voice-volume and gain arrays, replacing five register
 * bindings, byte-cursor accesses, and an explicit assembly move. Its
 * `void(s32)` contract and `SpuSetVoiceAttr` call are unchanged.
 *
 * Initializing the attribute-mask field at `+0x3C8` before the voice-mask field
 * at `+0x3C4` in C gives GCC the required register allocation. GCC schedules the
 * resulting machine stores in retail order, with the voice-mask store first.
 * Reversing those source assignments changes five instruction words. A split
 * profile alone leaves two wrong high-address register words, so the accepted
 * source does not add a compiler profile or change the global declaration.
 *
 * The conditional voice-mask shift, both unsigned fixed-point volume products,
 * their eight-bit shifts and halfword stores, and the final SDK attribute
 * publication remain intact. The original six-row canonical history and
 * six-row refinement history ending in deferral are preserved, with one new
 * post-terminal resolution for the source-order discriminator.
 */
#include "../types.h"
#include "sound.h"
#include "sound_voice_selection.h"

void func_80047864(s32 index)
{
    u32 mask = SD_VOICE_SLOT_MASK_BASE;
    SDValue *state;
    u32 product;

    if (index != 0) {
        mask <<= index;
    }
    state = g_SDValue;
    state->field_03C8 = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
    state->field_03C4 = mask;
    {
        u32 first = state->voice_volume_left[index];
        product = first * (u32)state->voice_value[index];
    }
    state->field_03CC = product >> 8;
    product = state->voice_volume_right[index] * (u32)state->voice_value[index];
    state->field_03CE = product >> 8;
    SpuSetVoiceAttr((SpuVoiceAttr *)&state->field_03C4);
}
