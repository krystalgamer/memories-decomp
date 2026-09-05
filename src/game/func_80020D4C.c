#include "../types.h"

/* Per-frame update for an object that orbits a fixed base position
   (unk2C/unk2E) at a constant angular step of 0x30/frame, with radius
   arg0->unk28 (which also serves as its countdown timer -- it decays by
   2/frame). When the timer reaches 0: clears unk6C/unk24, snaps the current
   position (unk30/unk32) back to the base position in one 32-bit copy, and
   returns. Otherwise, while D_8009B0CC's bit 0 is set, spawns a companion
   slot object at the object's current position (tagged via
   sub_table_lookup_set_flag using unk16-1), then advances the angle and
   recomputes the orbit position from base + (rcos,rsin)*radius/4096. */

struct Obj {
    u8 pad0[0x16];
    u8 unk16;
    u8 pad1[0x24 - 0x17];
    void (*unk24)(void *);
    s16 unk28;
    u16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    u8 pad2[0x40 - 0x34];
    u16 unk40;
    u8 pad3[0x68 - 0x42];
    u8 unk68;
    u8 unk69;
    u8 pad4[0x6C - 0x6A];
    u8 unk6C;
};

/* One entry of the D_800EFE48 pool; see get_or_init_D_800EFE48_slot.c. */
struct Slot {
    u16 f0;
    u16 f2;
    u32 f4;
    u16 f8;
    u8 pad0[0x16 - 0xA];
    u16 f16;
    u8 pad1[0x24 - 0x18];
    void (*f24)(void *);
    u8 pad2[0x40 - 0x28];
    u16 f40;
    u8 pad3[0x60 - 0x42];
    u16 f60;
};

extern s32 D_8009B0CC_arr[16];
#define D_8009B0CC (D_8009B0CC_arr[0])
extern u8 D_801AF000[16];

void func_80042BC0(void *);
s32 func_8004002C(void);
struct Slot *func_800400AC(s32, s32);
void func_800428A8(struct Slot *, s32, s32, s32, s32, s32, s32, s32, s32);
void func_800428EC(struct Slot *, s8);
int rsin(int);
int rcos(int);

void func_80020D4C(struct Obj *arg0) {
    s16 timer;
    u16 angle;
    struct Slot *slot;
    s32 vy;

    timer = arg0->unk28 - 2;
    arg0->unk28 = timer;
    if (timer <= 0) {
        arg0->unk6C = 0;
        arg0->unk24 = 0;
        *(s32 *) &arg0->unk30 = *(s32 *) &arg0->unk2C;
        return;
    }

    if (D_8009B0CC & 1) {
        slot = func_800400AC(func_8004002C(), 2);
        if (slot != 0) {
            func_800428A8(slot, arg0->unk30, arg0->unk32, 0,
                                   arg0->unk68, arg0->unk69, 0x11, 9,
                                   (s32) D_801AF000);
            slot->f40 = arg0->unk40 + 0x80;
            slot->f8 |= 0x28;
            slot->f4 |= 0x50000000;
            func_800428EC(slot, arg0->unk16 - 1);
            slot->f60 = 8;
            slot->f24 = func_80042BC0;
        }
    }

    angle = arg0->unk2A + 0x30;
    arg0->unk2A = angle;
    arg0->unk30 = arg0->unk2C + rcos((s16) angle) * arg0->unk28 / 4096;
    vy = rsin((s16) arg0->unk2A) * arg0->unk28;
    arg0->unk32 = arg0->unk2E + vy / 4096;
}
