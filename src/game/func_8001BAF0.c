#include "../types.h"
#include "ai_constants.h"
#include "card_constants.h"
#include "duel_card_layout.h"

typedef struct {
    u8 f0;
    u8 f1;
    u8 slot;
    u8 f3;
    s16 f4;
} DeckCardRecord;

typedef struct {
    u8 p0[0x30];
    s16 x;
    s16 y;
} Spawned;

typedef struct {
    Spawned *base;
    Spawned *child;
    u8 p8[4];
} Slot;

extern u8 *D_8009B1C8;
extern u8 D_8009B1D5;
extern u8 D_800EAE88[];
extern u8 D_800907CC[];
extern u8 D_801A7AD8[];
extern Slot D_800EA030[HAND_SIZE];
extern u8 gDuel_aActiveCards[];

extern void Duel_SetupCardRecord(s32, s32);
extern Spawned *func_80018004(u8 *, s32, s32);
extern void func_8004036C(Spawned *);

void func_8001BAF0(void)
{
    s8 sel[HAND_SIZE];
    DeckCardRecord tmp;
    register u8 *hand asm("$22");
    u8 *p;
    u8 *end;
    s8 *q;
    u8 *deck;
    register u8 *base asm("$8");
    u8 *tbl;
    u8 *recs;
    register DeckCardRecord *rec asm("$16");
    DeckCardRecord *other;
    Slot *slot;
    Spawned *spawned;
    u8 *card;
    s32 i;
    s32 j;
    register s32 v asm("$5");
    s32 id;
    s32 sidx;
    register s32 a asm("$5");
    s32 b;

    for (i = 0; i < HAND_SIZE; i++) {
        p = D_8009B1C8;
        sel[i] = *(u8 *)((0x1A + i) + (s32)p);
    }
    for (i = 0; i < HAND_SIZE; i++) {
        v = D_800EAE88[i];
        if (v == 0) {
            break;
        }
        if (v < 0x10) {
            p = (u8 *)&sel[v - 0xB];
            *(s8 *)p = -1;
        }
    }

    base = gDuel_aActiveCards;
    deck = base - 0x31E0;
    hand = D_800EAE88;
    end = D_800EAE88;
next:
    v = *hand;
    if (v == 0) {
        return;
    }
    if (v >= 0x10) {
        j = 0;
        tbl = D_800907CC;
        recs = D_801A7AD8;
        slot = D_800EA030;
    inner:
        q = &sel[j];
        sidx = *q;
        if (sidx >= 0) {
            {
                s32 o;

                o = v * AI_ACTIVE_CARD_RECORD_SIZE;
                {
                    register u8 *act asm("$9") = gDuel_aActiveCards;

                    card = act + o;
                }
            }
            rec = (DeckCardRecord *)(sidx * 6 + (s32)deck);
            other = (DeckCardRecord *)(card[0xB] * 6 + (s32)deck);
            a = *(s8 *)&rec->slot;
            b = other->slot;
            rec->slot = b;
            other->slot = a;
            tmp = *rec;
            *rec = *other;
            *other = tmp;
            {
                s32 k;

                k = D_8009B1D5 * HAND_SIZE;
                id = *(u8 *)((j + k) + (s32)tbl);
            }
            spawned = slot->base;
            Duel_SetupCardRecord(id, *(s8 *)&rec->slot);
            slot->base = func_80018004((u8 *)(id * DUEL_CARD_RECORD_SIZE + (s32)recs), spawned->x, spawned->y);
            func_8004036C(spawned);
            *(u8 *)((s32)D_8009B1C8 + (0x1A + j)) = rec->slot;
            *hand = j + 0xB;
            *q = -1;
        } else {
            j++;
            slot++;
            if (j < HAND_SIZE) {
                goto inner;
            }
        }
    }
    hand++;
    if ((s32)hand < (s32)(end + HAND_SIZE)) {
        goto next;
    }
}
