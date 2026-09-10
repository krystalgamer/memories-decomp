/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 5 variables to hard registers and 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_voice_selection.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/func_80047788.h"
#include "../game/sound.h"
#include "../game/sound_output_state.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"

void func_80047864(s32 index)
{
    register s32 saved asm("$5");

    asm volatile("move %0,%1" : "=r"(saved) : "r"(index));
    {
        register u32 mask asm("$3") = SD_VOICE_SLOT_MASK_BASE;
        u8 *state;
        register u8 *half asm("$3");
        register u8 *byte asm("$6");
        u32 product;

        if (saved != 0)
            mask <<= saved;
        state = (u8 *)g_SDValue;
        *(u32 *)(state + 0x3C4) = mask;
        half = state + (saved << 1);
        byte = state + saved;
        *(s32 *)(state + 0x3C8) = SPU_VOICE_VOLL | SPU_VOICE_VOLR;
        {
            register u32 first asm("$5") = *(u16 *)(half + 0x414);
            product = first * (u32)byte[0x424];
        }
        *(u16 *)(state + 0x3CC) = product >> 8;
        product = *(u16 *)(half + 0x41C) * (u32)byte[0x424];
        *(u16 *)(state + 0x3CE) = product >> 8;
        SpuSetVoiceAttr((SpuVoiceAttr *)(state + 0x3C4));
    }
}

