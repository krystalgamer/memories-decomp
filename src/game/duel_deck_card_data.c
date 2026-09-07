#include "../types.h"
#include "../psyq/qsort.h"
#include "card_constants.h"

typedef struct {
    u16 id;
    u8 b2;
    u8 b3;
    u8 b4;
    u8 b5;
} DeckCardRecord;

extern u8 D_8017808C[];
extern u8 D_80178130[];
extern u8 D_8015C424[];
extern u8 D_8018C2D8[];
extern DeckCardRecord gDuel_aDeckCardRecords[];
extern u32 D_8009B0F4;

extern void Util_CopyWords(void *, void *, s32);
extern s32 Util_CompareS16(s16 *, s16 *);
extern s32 func_800245EC(s32, s32);
extern u8 *func_80014EEC(s32, u8 *, s32, s32, void *, s32, s32);

void func_80024734(void)
{
    u8 *source = D_8017808C;
    u8 *output;
    u8 *result;
    s32 i;
    s32 previous;
    s32 count;
    u8 *table;
    s32 value;

    Util_CopyWords(source, source - 0xA4, COMBINED_DECK_SIZE * sizeof(u16));
    qsort(source, COMBINED_DECK_SIZE, sizeof(u16), (int (*)())Util_CompareS16);

    output = source + 0xA4;
    previous = 0;
    for (i = 0; i < COMBINED_DECK_SIZE; i++) {
        value = *(u16 *)source;
        if (value != previous) {
            *(u16 *)output = value;
            previous = value;
            output += 2;
        }
        source += 2;
    }

    *(u16 *)output = 0xFFFF;
    table = D_8015C424;
    count = *(u16 *)(table + 0x1BD0C);
    result = func_80014EEC(
        0, (u8 *)0, count - 1, previous - count + 1,
        func_800245EC, 0, 0
    );
    *(s32 *)(result + 0x38) = (s32)D_80178130;
    *(s32 *)(result + 0x3C) = (s32)table;
    D_8009B0F4 = *(s32 *)(result + 0x2C) | 0x10;
}

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
