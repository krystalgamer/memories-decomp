#include "../types.h"
#include "func_80043178.h"
#include "display_object_interpolation.h"
#include "display_object_api.h"

#include "duel_effect.h"
#include "display_object_helpers.h"
#include "text_box_runtime.h"
#include "func_80039794.h"

extern u8 D_8009AF76[];
extern u8 D_8009B3C1;
extern u8 D_8009B3C7;
extern s8 gDialog_bChoice __attribute__((section(".data")));
extern u16 D_8009AF76_raw asm("D_8009AF76")
    __attribute__((section(".data")));
extern u8 D_800EB0F8_raw[] asm("D_800EB0F8");

extern void func_80042C08();
extern u8 *DuelEffect_CreateChannel(s32, s32);

void func_8003D518(u8 *state)
{
    u8 *object;
    s32 flags;

    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        object = func_800400AC(func_8004002C(), 6);
        *(s16 *)(object + 48) = 160;
        *(s16 *)(object + 50) = 120;
        *(s16 *)(object + 72) = 128;
        *(s16 *)(object + 74) = 224;
        func_80042918(object);
        func_800428EC(object, (s8)(D_8009AF76[0] - 3));
        *(s32 *)(object + 76) = (s32)func_80042C08;
        *(u8 **)(state + 4) = object;
    }
    flags = D_8009B3C1;
    object = *(u8 **)(state + 4);
    if (flags & 64) {
        D_8009B3C1 = 0;
    } else {
        s32 x = *(u16 *)(object + 72) - 8;
        s32 y = *(u16 *)(object + 74) - 8;

        *(u16 *)(object + 74) = y;
        *(u16 *)(object + 72) = x;
        if ((s16)x <= 0) {
            D_8009B3C1 = flags | 64;
            *(s16 *)(object + 72) = 0;
            *(s16 *)(object + 74) = 64;
        }
    }
}

void func_8003D614(u8 *state)
{
    u8 *object;
    DuelEffectChannel *entry;

    if (!(D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED)) {
        object = *(u8 **)state;
        D_8009B3C1 |= DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        func_80043178((DisplayObjectSnapshot *)object);
        *(u16 *)(object + 0x60) = 0x400;
    }
    entry = &D_800EB0F8[state[0x1A]];
    object = *(u8 **)state;
    if (object) {
        *(u16 *)(object + 0x60) -= 0x40;
        if (*(s16 *)(object + 0x60) <= 0) {
            func_8004036C(object);
            *(void **)state = 0;
        } else {
            func_80043230(
                (DisplayObjectPosition *)                object,
                0x20,
                -0x40,
                *(s16 *)(object + 0x60)
            );
            TextBox_SetPos(
                entry,
                *(s16 *)(object + 0x30),
                *(s16 *)(object + 0x32)
            );
        }
    }
    object = *(u8 **)(state + 4);
    if (object) {
        *(u16 *)(object + 0x4A) += 8;
        *(u16 *)(object + 0x48) += 8;
        if (*(s16 *)(object + 0x48) >= 0xC0) {
            func_8004036C(object);
            *(void **)(state + 4) = 0;
        }
    }
    if (!*(void **)state && !*(void **)(state + 4))
        D_8009B3C1 = 0;
}

void func_8003D74C(u8 *o)
{
    u8 *p;
    u8 *r;
    s32 f;
    s32 v;
    s32 t;
    s32 a;
    s32 g;

    if ((D_8009B3C1 & DUEL_EFFECT_STATE_FLAG_INITIALIZED) == 0) {
        D_8009B3C1 =
            D_8009B3C1 | DUEL_EFFECT_STATE_FLAG_INITIALIZED;
        p = func_800400AC(func_8004002C(), 6);
        *(s16 *)(p + 0x30) = 0xA0;
        *(s16 *)(p + 0x32) = 0x78;
        *(s16 *)(p + 0x48) = 0x80;
        *(s16 *)(p + 0x4A) = 0xE0;
        func_80042918(p);
        func_800428EC(p, (s8)(*(u8 *)&D_8009AF76_raw - 3));
        *(u8 **)(p + 0x4C) = (u8 *)func_80042C08;
        *(u8 **)(o + 4) = p;
        p = func_800400AC(func_8004002C(), 2);
        func_800404CC(p, 0x20, -0x40, 3, 2, 0, 0xB, 0x20C);
        *(u16 *)(p + 8) = *(u16 *)(p + 8) | 0x28;
        func_80042918(p);
        func_800428EC(p, (s8)(*(u8 *)&D_8009AF76_raw - 2));
        *(u8 **)o = p;
        func_80043178((DisplayObjectSnapshot *)p);
        a = D_8009B3C7;
        *(s16 *)(p + 0x60) = -0x400;
        r = DuelEffect_CreateChannel((a & 1) | 0xD0, 0);
        *(u16 *)(r + 0x34) = *(u16 *)(r + 0x34) | 4;
        do {
            func_80039794();
        } while (*(s32 *)(r + 0x30) == 0);
        TextBox_SetPos(r, *(s16 *)(p + 0x30), *(s16 *)(p + 0x32));
    }
    f = D_8009B3C1;
    if ((f & 0x20) != 0) {
        D_8009B3C1 = 0;
        return;
    }
    r = D_800EB0F8_raw + o[0x1A] * 100;
    if ((f & 0x40) != 0) {
        func_80039794();
        if ((*(u16 *)(r + 0x34) & 0x2000) != 0) {
            D_8009B3C1 = D_8009B3C1 | 0x20;
            if (gDialog_bChoice == 0) {
                DuelEffect_CreateChannel(0x80D4, 0);
                gDialog_bChoice = 0;
            }
        }
        return;
    }
    p = *(u8 **)o;
    t = *(u16 *)(p + 0x60);
    if (*(s16 *)(p + 0x60) < 0) {
        v = t + 0x40;
        *(s16 *)(p + 0x60) = v;
        if ((s16)v >= 0) {
            *(s16 *)(p + 0x30) = 0x20;
            *(s16 *)(p + 0x60) = 0;
            *(s16 *)(p + 0x32) = 0x50;
        } else {
            func_80043230((DisplayObjectPosition *)p, 0x20, 0x50, (s16)v);
        }
        TextBox_SetPos(r, *(s16 *)(p + 0x30), *(s16 *)(p + 0x32));
    }
    p = *(u8 **)(o + 4);
    *(u16 *)(p + 0x4A) = *(u16 *)(p + 0x4A) - 8;
    *(u16 *)(p + 0x48) = *(u16 *)(p + 0x48) - 8;
    if (*(s16 *)(p + 0x48) <= 0) {
        *(u16 *)(p + 0x48) = 0;
        *(u16 *)(p + 0x4A) = 0x40;
        if (*(s16 *)(*(u8 **)o + 0x60) == 0) {
            g = D_8009B3C1;
            *(u16 *)(r + 0x34) = *(u16 *)(r + 0x34) & 0xFFFB;
            D_8009B3C1 = g | 0x40;
        }
    }
}
