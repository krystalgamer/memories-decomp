#include "../types.h"
#include "../psyq/rand.h"
#include "card_constants.h"

extern u8 gDuel_awOpponentDeckPool[];

s32 Rand_GetInterval(s32 arg0);

void Duel_ShuffleDeck(s32 src, u8 *out16, u8 *out8) {
    u8 buf[CARD_COUNT];
    u8 *b16;
    u8 *b8;
    u8 *e;
    u8 *t;
    u8 *q;
    s32 i;
    s32 n;
    s32 acc;
    s32 lim;
    s32 x;
    s32 y;
    s32 t16;
    s32 t8;
    s32 u16b;
    s32 u8b;

    b16 = out16;
    b8 = out8;

    if (src == 0) {
        for (i = CARD_COUNT - 1; i >= 0; i--) {
            buf[i] = 0;
        }

        t = gDuel_awOpponentDeckPool;
        n = 0;
        while (n < DECK_SIZE) {
            lim = (rand() & (DUEL_DROP_WEIGHT_TOTAL - 1)) + 1;
            acc = 0;
            i = 0;
            e = t;
            while (1) {
                acc += *(u16 *)e;
                if (acc >= lim) {
                    q = buf + i;
                    if (*q < DECK_CARD_COPY_LIMIT) {
                        *(s16 *)out16 = i + 1;
                        *out8 = n;
                        n++;
                        out16 += 2;
                        out8 += 1;
                        /* `t16` is the block-copy temp forty lines down; its
                         * live range resumes there, and borrowing the name
                         * here is 10 differences against 4. A fresh local is
                         * not the same thing (the permuter found this). */
                        t16 = *q + 1;
                        *q = t16;
                    }
                    break;
                }
                i++;
                e += 2;
                if (i >= STARTER_DECK_WEIGHT_SCAN_COUNT) {
                    break;
                }
            }
        }
    } else {
        for (i = 0; i < DECK_SIZE; i++) {
            *(s16 *)out16 = *(u16 *)src;
            src += 2;
            *out8 = i;
            out8++;
            out16 += 2;
        }
    }

    for (i = 0; i < DUEL_DECK_SHUFFLE_SWAP_COUNT; i++) {
        x = Rand_GetInterval(DECK_SIZE);
        y = Rand_GetInterval(DECK_SIZE);
        t16 = t8 = *(u16 *)(b16 + x * 2);
        u16b = *(u16 *)(b16 + y * 2);
        *(s16 *)(b16 + x * 2) = u16b;
        *(s16 *)(b16 + y * 2) = t16;
        t8 = b8[x];
        u8b = b8[y];
        b8[x] = u8b;
        b8[y] = t8;
    }
}
