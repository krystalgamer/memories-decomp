
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

typedef struct {
    int first;
    int second;
    short third;
    short fourth;
    u8 padC[12];
} Packet;

extern void func_80049594(s32);
extern void func_80049600(s32);
extern void func_80049544(void);

void func_80048F14(void)
{
    Packet packet;
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

    SpuReserveReverbWorkArea(1);
    SpuSetReverb(1);
    packet.first = 7;
    packet.second = 2;
    packet.third = 0x7FFF;
    packet.fourth = 0x7FFF;
    SpuSetReverbModeParam((SpuReverbAttr *)&packet);
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
