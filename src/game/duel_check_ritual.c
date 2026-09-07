#include "../types.h"
#include "card_constants.h"
#include "duel_card_layout.h"
#include "duel_grid.h"

typedef struct {
    u32 w0;
    u8 pad4[0x8];
    s16 id;
    u8 padE[0x8];
    u16 flags;
    u8 pad18[0x4];
} Card;

typedef struct {
    u32 w[DUEL_RITUAL_TRIBUTE_COUNT];
    s32 unkC;
} RitualOut;

extern u16 gDuel_awRitualData[];
extern u8 D_8009B1D5;
extern Card D_801A7AD8[];

u16 Duel_CheckRitual(RitualOut *out, s32 ritualId)
{
    Card *found[DUEL_RITUAL_TRIBUTE_COUNT];
    Card *cands[DUEL_FIELD_ROW_SIZE];
    Card *card;
    Card **first;
    Card **dst;
    register Card **w __asm__("$3");
    Card *c;
    u16 *p;
    u16 *q;
    s32 i;
    s32 j;

    p = gDuel_awRitualData;
    while (1) {
        if (p[0] == 0) {
            return 0;
        }
        if (p[0] == ritualId) {
            break;
        }
        p += DUEL_RITUAL_RECIPE_HALFWORD_COUNT;
    }

    i = DUEL_FIELD_ROW_SIZE;
    if (D_8009B1D5 != 0) {
        i = DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE;
    }
    c = &D_801A7AD8[i];
    i = 0;
    w = cands;
    for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
        *w = 0;
        if (c->flags & DUEL_CARD_FLAG_OCCUPIED) {
            *w = c;
        }
        w++;
        c++;
    }

    p++;
    j = 0;
    first = cands;
    dst = found;
    q = p;
    for (j = 0; j < DUEL_RITUAL_TRIBUTE_COUNT; j++) {
        for (i = 0; i < DUEL_FIELD_ROW_SIZE; i++) {
            card = first[i];
            if (card != 0 && card->id == q[0]) {
                goto matched;
            }
        }
        return 0;
matched:
        *dst++ = card;
        first[i] = 0;
        q++;
    }

    if (out != 0) {
        for (i = 0; i < DUEL_RITUAL_TRIBUTE_COUNT; i++) {
            out->w[i] = found[i]->w0;
        }
        out->unkC = 0;
    }
    return p[DUEL_RITUAL_TRIBUTE_COUNT];
}
