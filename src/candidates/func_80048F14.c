/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g0 this
 * source rebuilt the target byte for byte, but only by
 * pinning 1 variable to hard registers, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/sound_voice_data.c.
 */
#include "../types.h"
#include "../psyq/libspu.h"
#include "../game/sound.h"
#include "../game/sound_mix.h"
#include "../game/sound_pending_entries.h"
#include "../game/sound_voice_selection.h"

#include "../game/sound_init.h"
#include "../game/sound_voice_data.h"

void func_80048F14(void)
{
    SpuReverbAttr packet;
    /* Pinned: the first g_SDValue load's delay slot is what decides this
       function. Unpinned, GCC fills it with the 0xFF constant, which extends
       that value's live range across the three stores and pushes this pointer
       to $a2, shifting $v0/$a2/$a1 throughout. Pinning the pointer restores
       retail's fill, `lui $a0, 0x801E`. */
    register SDValue *a __asm__("$2");
    SDValue *c;
    u16 *base;
    u16 *r1;
    u16 *r2;
    u16 *r3;
    u16 *r4;
    u16 *r5;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    packet.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    packet.mode = SPU_REV_MODE_STUDIO_A;
    packet.depth.left = 0x7FFF;
    packet.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&packet);
    a = g_SDValue;
    a->field_1586 = 0;
    a->field_1588 = 0;
    ((u8 *)a)[0x158A] = 0;
    a = g_SDValue;
    *(s16 *)((u8 *)a + 0x1580) = 0xFF;
    a->field_1584 = 0xFF;
    c = g_SDValue;
    a->field_1582 = 0;
    base = (u16 *)0x801EA800;
    c->music_track = base;
    c->field_1560 = (u8 *)0x801E2000;
    c->field_1578 = -1;
    c->field_157A = -1;
    *(s16 *)((u8 *)c + 0x157C) = -1;
    c->field_157E = -1;
    r1 = c->music_track;
    *r1 = 0xFFFF;
    r2 = c->music_track;
    *(s16 *)((u8 *)r2 + 2) = 0;
    r3 = c->music_track;
    *(s32 *)((u8 *)r3 + 4) = 0;
    r4 = c->music_track;
    *(s32 *)((u8 *)r4 + 8) = 0;
    r5 = c->music_track;
    *(s32 *)((u8 *)r5 + 0xC) = 0x40000;
    func_80049594(2);
    func_80049600(0x14);
    func_80049544();
}
