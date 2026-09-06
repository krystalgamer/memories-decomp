#include "../types.h"
#include "../psyq/rand.h"
#include "card_constants.h"
#include "model.h"

extern s32 D_8009B0F4 __attribute__((section(".data")));
extern s32 D_8009B134 __attribute__((section(".data")));

void func_8005922C(u8 *arg0, s32 arg1);

void func_80050584(s32 arg0) {
    u8 *p;
    u8 *b;
    u8 *b0;
    u8 *q;
    u8 *r;
    u8 *s;
    s32 v;
    s32 t;
    s32 a;
    s32 m1;

    b0 = (u8 *)D_800F2C40;
    p = b0 + arg0 * MODEL_SLOT_SIZE;
    if (p[0xE1F] == 0) {
        if (p[0xE14] == 0xFF) {
            if (((D_8009B0F4 & 0x2000030) | D_8009B134) == 0) {
                do {
                    t = rand() >> 8;
                    v = t % CARD_COUNT;
                } while (v < 0 || v >= CARD_COUNT ||
                         (v >= 0x12C && v < 0x15E) ||
                         (v >= 0x28A && v < 0x2BC) ||
                         v == 0x2D0);
                m1 = -1;
                Model_LoadMonsterMerge(arg0 | 0x80, v, m1, m1, m1, m1, 0);
            }
        } else {
            func_80056828(arg0);
        }

        b = (u8 *)D_800F2C40;
        if ((b + arg0 * MODEL_SLOT_SIZE)[0xE1F] != 0) {
            q = b + (arg0 ^ 1) * MODEL_SLOT_SIZE;
            a = MODEL_ANGLE_QUARTER_TURN;
            if (q[0xE1F] != 0) {
                r = *(u8 **)(q + 0xD18);
                if (r != (u8 *)0) {
                    t = *(s16 *)(r + 0x46) + MODEL_ANGLE_QUARTER_TURN;
                    a = t / MODEL_ANGLE_FULL_TURN;
                    a = t - a * MODEL_ANGLE_FULL_TURN;
                }
            }
            s = (u8 *)D_800F2C40 + arg0 * MODEL_SLOT_SIZE;
            if (*(s32 *)(s + 0xD18) != 0) {
                *(s16 *)(*(s32 *)(s + 0xD18) + 0x44) = 0;
                *(s16 *)(*(s32 *)(s + 0xD18) + 0x46) = a;
                *(s16 *)(*(s32 *)(s + 0xD18) + 0x48) = 0;
                *(s32 *)(*(s32 *)(s + 0xD18) + 0x18) = 0;
                *(s32 *)(*(s32 *)(s + 0xD18) + 0x1C) = 0;
                *(s32 *)(*(s32 *)(s + 0xD18) + 0x20) = 0;
            }
            func_8005922C(*(u8 **)(s + 0xD18), 0);
            func_80059DD8(arg0);
            s[0xE15] = 0;
        }
    }
}
