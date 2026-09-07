#include "../types.h"
#include "card_constants.h"

typedef struct {
    u16 id;
    u8 b2;
    u8 b3;
    u8 b4;
    u8 b5;
} DeckCardRecord;

extern u8 D_8015C424[];
extern u8 D_80178130[];
extern u8 D_8018C2D8[];
extern DeckCardRecord gDuel_aDeckCardRecords[];

extern void Util_CopyWords(void *, void *, s32);

void func_80024824(void)
{
    u8 *dst = D_8018C2D8;
    DeckCardRecord *rec = gDuel_aDeckCardRecords;
    u8 *q;
    u8 *src;
    u8 *r;
    u16 *p;
    s32 i;
    s32 id;
    s32 w;
    u16 v;

    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        q = D_8015C424 + i * 2;
        v = *(u16 *)(q + 0x1BBC4);
        rec->id = v;
        id = (s16)v;
        r = D_8015C424 + i;
        rec->b4 = r[0x1BB70];
        rec->b2 = i;
        rec->b3 = i;

        src = D_8015C424;
        p = (u16 *)D_80178130;
    search:
        w = *p;
        p++;
        if (w != id) {
            src += 0x580;
            goto search;
        }
        Util_CopyWords(dst, src, 0x580);
        dst += 0x580;
        rec++;
    }
}
