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
