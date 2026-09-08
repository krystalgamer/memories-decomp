#include "../types.h"
#include "display_effect_lifecycle.h"
#include "menu_record.h"

typedef struct {
    u8 *streams[20];
    u8 unk50;
    u8 state;
    u8 pad52[6];
    s8 depth;
} EffectObject;

extern s32 D_8009B350;
extern u8 *D_8009B328;

/* Effect-script command handler: reads a command id and a flag byte from the
   object's current script stream, finds the display effect record for the id
   in D_800EB010 (ids below 0x41 live in the first two records, others in
   the third), and depending on the flags either stops it (bit 7), adjusts a
   running one (bits 5 and 6) or starts it in the slot given by bit 0, with
   the object's state byte set to the matching wait state. */
void func_80038EB0(EffectObject *o) {
    u8 *e;
    s32 id;
    s32 flags;
    s32 slot;
    s32 a;
    s32 b;

    D_8009B350 = 1;
    a = *o->streams[o->depth]++;
    b = *o->streams[o->depth]++;
    id = a;
    flags = b;

    e = (u8 *)D_800EB010;
    if (id >= 0x41) {
        e += 0x98;
    } else if (*(s8 *)(e + 0x30) != id) {
        e += 0x4C;
        if (*(s8 *)(e + 0x30) != id) {
            e = 0;
        }
    }

    if (flags & 0x80) {
        if (e == 0) {
            return;
        }
        D_8009B328 = e;
        if (flags & 2) {
            func_80039FD4((u8 *)e);
            return;
        }
        if (flags & 1) {
            o->state = 0xE;
            return;
        }
        if (e[0x3C] != 0) {
            *(s16 *)(e + 0x40) = 0x178;
        } else {
            *(s16 *)(e + 0x40) = -0x38;
        }
        *(s16 *)(e + 0x42) = 0xB2;
        *(s16 *)(e + 0x44) = 0x10;
        o->state = 7;
        e[0x33] = 3;
        if (id >= 0x41) {
            e[0x33] = 5;
            *(s16 *)(e + 0x40) = 1;
        }
        return;
    }
    if (flags & 0x60) {
        if (e == 0) {
            return;
        }
        if (id >= 0x41) {
            return;
        }
        D_8009B328 = e;
        if (flags & 0x40) {
            e[0x31] = flags & 3;
            o->state = 9;
            return;
        } else if (flags & 0x20) {
            e[0x32] &= 0xEF;
            if (flags & 1) {
                e[0x32] |= 0x10;
            }
            return;
        }
    }

    slot = flags & 1;
    if (id >= 0x41) {
        slot = 2;
    }
    e = (u8 *)D_800EB010 + slot * sizeof(MenuRecord);
    func_80039F44((DisplayEffectState *)e);
    e[0x30] = id;
    e[0x3C] = slot;
    if (slot != 0) {
        *(s16 *)(e + 0x34) = 0x178;
    } else {
        *(s16 *)(e + 0x34) = -0x38;
    }
    o->state = 6;
    D_8009B328 = e;
    if (id >= 0x41) {
        e[0x33] = 5;
        e[0x3C] = 2;
        *(s16 *)(e + 0x34) = 0xF0;
        *(s16 *)(e + 0x40) = 0;
        *(s16 *)(e + 0x36) = 0x60;
        return;
    }
    e[0x33] = 2;
    *(s16 *)(e + 0x40) = 3;
    if (flags & 8) {
        *(s16 *)(e + 0x34) = 0x400;
        *(s16 *)(e + 0x40) = 7;
    }
    if (flags & 0x10) {
        e[0x31] = (flags >> 1) & 3;
    }
}
