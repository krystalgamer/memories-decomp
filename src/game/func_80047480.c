#include "../types.h"
#include "func_80047480.h"
#include "sound.h"
#include "../psyq/libspu.h"
#include "sound_pending_constants.h"
#include "sound_voice_constants.h"

void func_80047480(void) {
    SDValue *p;
    SDValue *q;
    SDValue *r;
    s32 i;
    s32 j;
    s32 k;

    g_SDValue->voice_active_mask = 0;
    g_SDValue->field_0435 = 0;

    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++) {
        g_SDValue->field_040C[i] = 0;
        g_SDValue->voice_flags[i] = 0;
        g_SDValue->voice_ids[i] = 0;
        g_SDValue->voice_volume_left[i] = 0;
        g_SDValue->voice_volume_right[i] = 0;
        g_SDValue->voice_step[i] = 0;
        g_SDValue->voice_timer[i] = 0;
    }

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);

    /* The constant half of the staged voice attribute: unity volumex, unity
       pitch, middle-C sample note, linear attack and sustain with exponential
       release, and an all-bits attribute mask. */
    p = g_SDValue;
    p->voice_attr.volumex.left = 0x3FFF;
    p->voice_attr.volumex.right = 0x3FFF;
    p->voice_attr.pitch = 0x1000;
    p->voice_attr.sample_note = 0x3C00;
    p->voice_attr.a_mode = 1;
    p->voice_attr.s_mode = 1;
    p->voice_attr.r_mode = 3;
    p->field_0448 = (SDValueLink *)0x801E2800;
    p->voice_attr.mask = 0xFFFF;
    p->voice_attr.volmode.left = 0;
    p->voice_attr.volmode.right = 0;
    p->voice_attr.ar = 0;
    p->voice_attr.dr = 0;
    p->voice_attr.sr = 0;
    p->voice_attr.rr = 0;
    p->voice_attr.sl = 0;
    p->field_043C = (u16 *)0x801E4000;
    p->field_0438 = 0x1010;
    p->field_0440 = 0;
    p->field_0442 = 0xFFFF;
    p->field_0444 =
        (SDNote *)(((p->field_0000 * 2 + 0xF) & 0xFFF0) + 0x801E4000);

    i = 0;
    if (p->field_0000 != 0) {
        r = p;
        do {
            r->field_043C[i] = SD_PENDING_ENTRY_NONE;
            i++;
        } while (i < r->field_0000);
    }

    i = 0;
    q = g_SDValue;
    for (; i < SD_VOICE_LOOKUP_BANK_COUNT; i++) {
        j = 0;
        k = i << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        for (; j < SD_VOICE_LOOKUP_BANK_ENTRY_COUNT; j++) {
            /* The bank shift is already folded into k, and retail adds the
               record base to it in that order, so this keeps the byte form
               rather than becoming q->field_044C[i][j]. */
            *(u16 *)((u8 *)q + k + SD_VOICE_LOOKUP_BYTE_OFFSET) =
                SD_PENDING_ENTRY_NONE;
            k += 2;
        }
    }
}
